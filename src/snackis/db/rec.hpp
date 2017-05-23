#ifndef SNACKIS_DB_REC_HPP
#define SNACKIS_DB_REC_HPP

#include <map>
#include <string>

#include "snackis/core/val.hpp"

namespace snackis {
namespace db {
  template <typename RecT>
  struct TableCol;

  template <typename RecT>
  using Rec = std::map<const TableCol<RecT> *, Val>;
}}

#endif
