#ifndef SNABEL_EXEC_HPP
#define SNABEL_EXEC_HPP

#include <map>

#include "snabel/fiber.hpp"

namespace snabel {  
  struct Exec {
    std::map<size_t, Fiber> fibers;
    Fiber &main;

    Type &i64_type, &str_type;
    
    Exec():
      main(fibers.emplace(std::piecewise_construct,
			  std::forward_as_tuple(0),
			  std::forward_as_tuple(*this, 0)).first->second),
      i64_type((add_type(main.root_ctx, "i64"))),
      str_type((add_type(main.root_ctx, "str")))
    { }

    Exec(const Exec &) = delete;
    const Exec &operator =(const Exec &) = delete;
  };
}

#endif
