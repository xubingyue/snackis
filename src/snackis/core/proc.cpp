#include <cstdlib>
#include "snackis/core/buf.hpp"
#include "snackis/core/proc.hpp"

namespace snackis {
  int run_proc(const str &cmd, std::initializer_list<str> args) {
    Buf buf;
    buf << cmd;
    for (const str &s: args) { buf << " " << s; }
    return system(buf.str().c_str());
  }
}
