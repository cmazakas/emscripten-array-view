#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <algorithm>
#include <utility>

using emscripten::function;
using emscripten::val;

struct helper
{
  val& v_;
  size_t pos_;

  helper(void) = delete;
  helper(val& v, size_t const pos) : v_{v}, pos_{pos} {};

  auto operator=(val v) -> void
  {
    v_.set(pos_, v);
  }
};

struct array_view
{
public:
  using value_type = decltype(std::declval<val>()[0]);
  using reference = value_type&;
  using pointer = value_type*;

private:
  val& v_;
  size_t size_;

public:
  array_view(void) = delete;
  array_view(val& v) : v_{v}, size_{0} {}

  auto operator[](size_t const pos) -> helper
  {
    return helper{v_, pos};
  }
};

float lerp(float a, float b, float t) 
{
  return (1 - t) * a + t * b;
}

auto sort_array(val v) -> val
{
  array_view av{v};

  av[0] = val{1337};

  return v;
}

EMSCRIPTEN_BINDINGS(my_module) 
{
  function("lerp", &lerp);
  function("sort_array", &sort_array);
}