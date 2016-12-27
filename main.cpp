#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <cassert>
#include <algorithm>
#include <utility>
#include <iterator>

template <typename T>
struct val_wrapper
{
  emscripten::val& v_;
  size_t pos_;

  val_wrapper(void) = delete;
  val_wrapper(emscripten::val& v, size_t const pos) : v_{v}, pos_{pos} {};

  auto operator=(T const& t) -> val_wrapper&
  {
    v_.set(pos_, t);
    return *this;
  }

  auto operator=(T&& t) -> val_wrapper&
  {
    v_.set(pos_, std::move(t));
    return *this;
  }
};

template <typename T>
struct helper
{
  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = helper;
  using reference = emscripten::val;
  using iterator_category = std::random_access_iterator_tag;

  emscripten::val& v_;
  size_t pos_;

  helper(void) = delete;
  helper(emscripten::val& v, size_t const pos) : v_{v}, pos_{pos} {};

  auto operator=(T v) -> void
  {
    v_.set(pos_, v);
  }

  auto operator<(helper<T> const& other) -> bool
  {
    return (v_[pos_].template as<T>() < other.v_[other.pos_].template as<T>());
  }

  auto operator>(helper const& other) -> bool
  {
    return false;
  }

  auto operator==(helper const& other) -> bool
  {
    return (v_ == other.v_ && pos_ == other.pos_);
  }

  auto operator!=(helper const& other) -> bool
  {
    return !(*this == other);
  }

  auto operator*(void) -> val_wrapper<T>
  {
    return val_wrapper<T>{v_, pos_};
  }

  auto operator+(size_t const pos) -> helper
  {
    return {v_, pos_ + pos};
  }
};

template <typename T>
struct array_view
{
public:
  using value_type = T;
  using reference = value_type&;
  using pointer = value_type*;

private:
  emscripten::val& v_;
  size_t size_;

public:
  array_view(void) = delete;
  array_view(emscripten::val& v) : v_{v}, size_{0} {}

  auto operator[](size_t const pos) -> helper<T>
  {
    return helper<T>{v_, pos};
  }

  auto begin(void) -> helper<T>
  {
    return helper<T>{v_, 0};
  }

  auto end(void) -> helper<T>
  {
    return helper<T>{v_, size_ - 1};
  }
};

float lerp(float a, float b, float t) 
{
  return (1 - t) * a + t * b;
}

auto sort_array(emscripten::val v) -> emscripten::val
{
  array_view<int> av{v};

  av[0] = 1337;
  av[1] = 1338;

  assert((av[0] < av[1]));

  auto begin = av.begin();
  *begin = 7331;

  //std::sort(av.begin(), av.end());

  return v;
}

EMSCRIPTEN_BINDINGS(my_module) 
{
  emscripten::function("lerp", &lerp);
  emscripten::function("sort_array", &sort_array);
}