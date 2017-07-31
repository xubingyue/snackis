#ifndef SNABEL_EXEC_HPP
#define SNABEL_EXEC_HPP

#include <map>

#include "snabel/fiber.hpp"
#include "snabel/type.hpp"
#include "snackis/core/uid.hpp"

namespace snabel {
  struct Exec {
    std::map<UId, Fiber> fibers;
    Fiber &main;
    Type &i64_type, &op_type, &op_seq_type, &str_type;
    
    Exec():
      main(fibers.emplace(std::piecewise_construct,
			  std::forward_as_tuple(null_uid),
			  std::forward_as_tuple(*this, null_uid)).first->second),
      i64_type((add_type(get_ctx(main), "i64"))),
      op_type((add_type(get_ctx(main), "op"))),
      op_seq_type((add_type(get_ctx(main), "op_seq"))),
      str_type((add_type(get_ctx(main), "str")))
    { }

    Exec(const Exec &) = delete;
    const Exec &operator =(const Exec &) = delete;
  };
}

#endif
