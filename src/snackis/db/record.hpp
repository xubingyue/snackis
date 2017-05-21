#ifndef SNACKIS_RECORD_HPP
#define SNACKIS_RECORD_HPP

#include <map>
#include <string>

#include "snackis/core/value.hpp"

namespace snackis {
  template <typename RecT>
  struct TableColumn;

  template <typename RecT>
  using Record = std::map<const TableColumn<RecT> *, Value>;
}

#endif
