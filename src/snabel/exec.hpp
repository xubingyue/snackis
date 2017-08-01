#ifndef SNABEL_EXEC_HPP
#define SNABEL_EXEC_HPP

#include <map>

#include "snabel/fiber.hpp"
#include "snackis/core/uid.hpp"

namespace snabel {
  struct Type;
  
  struct Exec {
    std::map<UId, Fiber> fibers;
    Fiber &main;
    Ctx &ctx;
    Type &meta_type,
      &op_type, &op_seq_type,
      &func_type, &i64_type, &str_type;
    
    Exec();
    Exec(const Exec &) = delete;
    const Exec &operator =(const Exec &) = delete;
  };

  void compile(Exec &exe, const str &in);
  void run(Exec &exe);
}

#endif
