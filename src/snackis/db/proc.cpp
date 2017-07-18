#include "snackis/db/proc.hpp"

namespace snackis {
namespace db {
  Proc::Proc(const Path &p, size_t max_buf):
    Loop(*this, max_buf), path(p)
  {
    create_path(path);
  }
}}
