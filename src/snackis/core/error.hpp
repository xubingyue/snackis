#ifndef SNACKIS_ERROR_HPP
#define SNACKIS_ERROR_HPP

#include <stdexcept>

#include "snackis/core/fmt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/macros.hpp"
#include "snackis/core/trace.hpp"

#define ERROR(type, msg) {					\
    TRACE("Error thrown");					\
    throw CONCAT(type, Error)(msg);				\
  }								\

#define CHECK(expr, cond) {						\
    auto _(expr);							\
									\
    if (!(cond)) {							\
      ERROR(snackis::Core,						\
	    fmt("Check failed: %0 (%1 = %2)", #cond, #expr, _));	\
    }									\
  }									\
  
namespace snackis {
  struct Error: std::runtime_error {
    Error(const str &msg);
  };

  struct CoreError: Error {
    CoreError(const str &msg);
  };
}

#endif
