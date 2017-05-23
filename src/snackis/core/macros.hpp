#ifndef MACROS_HPP
#define MACROS_HPP

#define _CONCAT(x, y)				\
  x ## y					\

#define CONCAT(x, y)				\
  _CONCAT(x, y)					\

#endif
