#ifndef SNACKIS_TRACE_HPP
#define SNACKIS_TRACE_HPP

#define TRACE(msg)				\
  Trace UNIQUE(trace)(msg, __FILE__, __LINE__)	\

#include <string>

namespace snackis {
  struct Trace {
    const std::string msg;
    const char *file;
    const int line;

    Trace(const std::string &msg, const char *file, int line);
    ~Trace();
  };

  std::string stack_trace();
}

#endif
