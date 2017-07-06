#ifndef SNACKIS_ERROR_HPP
#define SNACKIS_ERROR_HPP

#include "snackis/core/fmt.hpp"
#include "snackis/core/func.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/macros.hpp"
#include "snackis/core/trace.hpp"

#define ERROR(type, what) {			\
    TRACE("Error thrown");			\
    throw_error(new CONCAT(type, Error)(what));	\
  }						\

#define CHECK(expr, cond) {						\
    auto _(expr);							\
									\
    if (!(cond)) {							\
      ERROR(snackis::Core,						\
	    fmt("Check failed: %0 (%1 = %2)",				\
		#cond, #expr, _));					\
    }									\
  }									\

#define TRY(id)					\
  Try id(#id, __FILE__, __LINE__)		\

#define CATCH(t, etype, var)			\
  for (auto var(catch_error<etype>(t));		\
       var;					\
       delete var, var = catch_error<etype>(t))	\

namespace snackis {  
  struct Error {
    const str what;
    Error(const str &what);
  };

  struct CoreError: Error {
    CoreError(const str &what);
  };

  struct Try: Trace {
    std::vector<Error *> errors;

    Try(const str &id, const str &file, int line);
    ~Try();
  };

  using ErrorHandler = func<void (const std::vector<Error *> &)>;
  extern thread_local ErrorHandler error_handler;
  
  void throw_error(Error *e);

  template <typename ET>
  ET *catch_error(Try &t) {
    for (auto i(t.errors.begin()); i != t.errors.end(); i++) {
      auto e(dynamic_cast<ET *>(*i));

      if (e) {
	t.errors.erase(i);
	return e;
      }
    }

    return nullptr;
  }
}

#endif
