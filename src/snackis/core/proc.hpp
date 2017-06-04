#ifndef SNACKIS_PROC_HPP
#define SNACKIS_PROC_HPP

#include <initializer_list>
#include "snackis/core/str.hpp"

namespace snackis {
  int run_proc(const str &cmd, std::initializer_list<str> args);
}

#endif
