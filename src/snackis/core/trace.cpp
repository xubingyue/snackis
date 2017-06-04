#include <iostream>
#include <vector>

#include "snackis/core/stream.hpp"
#include "snackis/core/trace.hpp"

namespace snackis {
  thread_local std::vector<const Trace *> stack;
  
  Trace::Trace(const str &msg, const char *file, int line):
    msg(msg), file(file), line(line) {
    stack.push_back(this);
  }

  Trace::~Trace() {
    stack.pop_back();
  }
  
  str stack_trace() {
    Stream out;

    for (const Trace *t: stack) {
      out << t->msg << " in file " << t->file << ", line " << t->line << ":\n";
    }
    
    return out.str();
  }

}
