#ifndef MACROS_HPP
#define MACROS_HPP

#define _CONCAT(x, y)				\
  x ## y					\

#define CONCAT(x, y)				\
  _CONCAT(x, y)					\

#define UNIQUE(prefix)				\
  CONCAT(prefix, __COUNTER__)			\

#endif
