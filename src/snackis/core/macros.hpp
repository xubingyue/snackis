#ifndef SNACKIS_MACROS_HPP
#define SNACKIS_MACROS_HPP

#define _CONCAT(x, y)				\
  x ## y					\

#define CONCAT(x, y)				\
  _CONCAT(x, y)					\

#define UNIQUE(prefix)				\
  CONCAT(prefix, __LINE__)			\

#endif
