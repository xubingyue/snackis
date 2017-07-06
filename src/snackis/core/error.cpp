#include <cassert>
#include <iostream>
#include "snackis/core/error.hpp"

namespace snackis {
  static void default_handler(const std::vector<Error *> &errors) {
    for (auto e: errors) { std::cerr << e->what << std::endl; }
    abort();
  }
  
  thread_local ErrorHandler error_handler(default_handler);

  static thread_local std::vector<Try *> try_stack;

  Error::Error(const str &what):
    what(stack_trace() + what)
  { }

  CoreError::CoreError(const str &what):
    Error(fmt("CoreError: %0", what)) { }

  Try::Try(const str &id, const str &file, int line):
    Trace(id, file, line)
  {
    try_stack.push_back(this);
  }
  
  Try::~Try() {
    assert(!try_stack.empty());
    assert(try_stack.back() == this);
    try_stack.pop_back();

    if (try_stack.empty()) {
      if (!errors.empty()) {
	error_handler(errors);
	for (auto e: errors) { delete e; }
      }
    } else {
      std::copy(errors.begin(), errors.end(),
		std::back_inserter(try_stack.back()->errors));
    }
  }
  
  void throw_error(Error *e) {
    assert(!try_stack.empty());
    assert(e);
    try_stack.back()->errors.push_back(e);
  }
}
