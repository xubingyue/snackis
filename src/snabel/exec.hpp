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
    Type &meta_type,
      &op_type, &op_seq_type,
      &func_type, &i64_type, &str_type;
    
    Exec():
      main(fibers.emplace(std::piecewise_construct,
			  std::forward_as_tuple(null_uid),
			  std::forward_as_tuple(*this, null_uid)).first->second),
      meta_type((add_type(get_ctx(main), "Type"))),
      op_type((add_type(get_ctx(main), "Op"))),
      op_seq_type((add_type(get_ctx(main), "OpSeq"))),
      func_type((add_type(get_ctx(main), "Func"))),
      i64_type((add_type(get_ctx(main), "I64"))),
      str_type((add_type(get_ctx(main), "Str")))
    {
      meta_type.fmt = [](auto &v) { return get<Type *>(v)->name; };
      op_type.fmt = [](auto &v) { return fmt_arg(get<Op>(v).code); };
      op_seq_type.fmt = [](auto &v) { return fmt_arg(get<OpSeq>(v).size()); };
      func_type.fmt = [](auto &v) { return fmt_arg(size_t(get<Func *>(v))); };
      i64_type.fmt = [](auto &v) { return fmt_arg(get<int64_t>(v)); };
      str_type.fmt = [](auto &v) { return get<str>(v); };
    }

    Exec(const Exec &) = delete;
    const Exec &operator =(const Exec &) = delete;
  };
}

#endif
