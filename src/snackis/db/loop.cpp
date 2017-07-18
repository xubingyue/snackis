#include "snackis/db/loop.hpp"

namespace snackis {
namespace db {
  Loop::Loop(size_t max_buf):
    inbox(max_buf)
  { }
}}
