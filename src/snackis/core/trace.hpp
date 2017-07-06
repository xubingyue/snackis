#ifndef SNACKIS_TRACE_HPP
#define SNACKIS_TRACE_HPP

#define TRACE(msg)				\
  Trace UNIQUE(trace)(msg, __FILE__, __LINE__)	\

#include "snackis/core/str.hpp"

namespace snackis {
  struct Trace {
    const str msg;
    const str file;
    const int line;

    Trace(const str &msg, const str &file, int line);
    virtual ~Trace();
  };

  str stack_trace();
}

#endif
