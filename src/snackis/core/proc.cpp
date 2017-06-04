#include <cstdlib>
#include "snackis/core/stream.hpp"
#include "snackis/core/proc.hpp"

namespace snackis {
  int run_proc(const str &cmd, std::initializer_list<str> args) {
    Stream buf;
    buf << cmd;
    for (const str &s: args) { buf << " " << s; }
    return system(buf.str().c_str());
  }
}
