#ifndef SNACKIS_RECORD_HPP
#define SNACKIS_RECORD_HPP

#include <map>
#include <string>

#include "snackis/core/value.hpp"
#include "snackis/db/table_column.hpp"

namespace snackis {
  using namespace std;
  
  template <typename RecT>
  using Record = map<const TableColumn<RecT> *, Value>;
}

#endif
