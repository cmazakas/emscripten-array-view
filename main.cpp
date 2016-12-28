#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <cassert>
#include <algorithm>
#include <utility>
#include <iterator>

struct val_wrapper
{
  emscripten::val& v_;
  size_t pos_;

  val_wrapper(void) = delete;
  val_wrapper(emscripten::val& v, size_t const pos) : v_{v}, pos_{pos} {};

  auto operator=(emscripten::val const& t) -> val_wrapper&
  {
    v_.set(pos_, t);
    return *this;
  }

  auto operator=(emscripten::val&& t) -> val_wrapper&
  {
    v_.set(pos_, std::move(t));
    return *this;
  }
};

template <typename T>
struct array_view_iterator
{
  using difference_type = ptrdiff_t;
  using value_type = emscripten::val;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::input_iterator_tag; 

  emscripten::val* v_;
  size_t pos_;

  array_view_iterator(emscripten::val* v, size_t pos)
    : v_{v}, pos_{pos}
  {}

  array_view_iterator(array_view_iterator const& avi)
    : v_{avi.v_}, pos_{avi.pos_}
  {}

  ~array_view_iterator(void) = default;

  auto operator=(array_view_iterator const& av_i) -> array_view_iterator&
  {
    v_ = av_i.v_;
    pos_ = av_i.pos_;
    return *this;
  }

  auto operator*(void) -> val_wrapper
  {
    return {*v_, pos_};
  }

  auto operator++(void) -> array_view_iterator&
  {
    ++pos_;
    return *this;
  }

  auto operator==(array_view_iterator const& av_iter) -> bool
  {
    return v_ == av_iter.v_ && pos_ == av_iter.pos_;
  }

  auto operator!=(array_view_iterator const& av_iter) -> bool
  {
    return v_ != av_iter.v_ || pos_ != av_iter.pos_;
  }  
};


auto av_iterator_tests(emscripten::val& v) -> emscripten::val&
{
  using it_traits = std::iterator_traits<array_view_iterator<int>>;
  using difference_type = typename it_traits::difference_type;
  using value_type = typename it_traits::value_type;
  using pointer = typename it_traits::pointer;
  using reference = typename it_traits::reference;
  using iterator_category = typename it_traits::iterator_category;

  array_view_iterator<int> av_iterator{&v, 0};
  array_view_iterator<int> other{&v, 13};

  std::swap(av_iterator, other);
  assert((std::distance(other, av_iterator) == 13));

  std::swap(av_iterator, other);
  assert((std::distance(av_iterator, other) == 13));

  *av_iterator = value_type{1337};
  *other = value_type{7331};

  return v;
}

auto sort_array(emscripten::val v) -> emscripten::val
{
  av_iterator_tests(v);
  return v;
}

EMSCRIPTEN_BINDINGS(emu) 
{
  emscripten::function("sort_array", &sort_array);
}