#include "snackis/core/trace.hpp"

#include <iostream>
#include <sstream>
#include <vector>

namespace snackis {
  thread_local std::vector<const Trace *> stack;
  
  Trace::Trace(const std::string &msg, const char *file, int line):
    msg(msg), file(file), line(line) {
    stack.push_back(this);
  }

  Trace::~Trace() {
    stack.pop_back();
  }
  
  std::string stack_trace() {
    std::stringstream out;

    for (const Trace *t: stack) {
      out << t->msg << " in file " << t->file << ", line " << t->line << ":\n";
    }
    
    return out.str();
  }

}
