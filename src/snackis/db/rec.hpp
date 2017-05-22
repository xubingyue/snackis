#ifndef SNACKIS_REC_HPP
#define SNACKIS_REC_HPP

#include <map>
#include <string>

#include "snackis/core/val.hpp"

namespace snackis {
  template <typename RecT>
  struct TableCol;

  template <typename RecT>
  using Rec = std::map<const TableCol<RecT> *, Val>;
}

#endif
