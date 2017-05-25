#ifndef SNACKIS_UTILS_HPP
#define SNACKIS_UTILS_HPP

namespace snackis {
  template <typename X, typename Y>
  X min(const X &x, const Y &_y) {
    const X y = _y;
    return (x < y) ? x : y;
  }

  template <typename X, typename Y>
  X max(const X &x, const Y &_y) {
    const X y = _y;
    return (x > y) ? x : y;
  }
}

#endif
