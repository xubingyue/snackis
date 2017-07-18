#include "snackis/db/loop.hpp"

namespace snackis {
namespace db {
  Loop::Loop(Proc &proc, size_t max_buf):
    proc(proc),
    inbox(max_buf)
  { }
}}
