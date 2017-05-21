#ifndef SNACKIS_TABLE_COLUMN_HPP
#define SNACKIS_TABLE_COLUMN_HPP

#include <string>
#include "snackis/db/record.hpp"

namespace snackis {
  template <typename RecT>
  struct TableColumn {
    const std::string name;
    TableColumn(const std::string &name);
    virtual void copy(Record<RecT> &dest, const RecT &src) const = 0;
  };

  template <typename RecT>
  TableColumn<RecT>::TableColumn(const std::string &name): name(name) {
  }
}

#endif
