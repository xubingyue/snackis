#ifndef SNACKIS_TABLE_COL_HPP
#define SNACKIS_TABLE_COL_HPP

#include <string>
#include "snackis/db/rec.hpp"

namespace snackis {
  template <typename RecT>
  struct TableCol {
    const std::string name;
    TableCol(const std::string &name);
    virtual void copy(Rec<RecT> &dest, const RecT &src) const = 0;
  };

  template <typename RecT>
  TableCol<RecT>::TableCol(const std::string &name): name(name) { }
}

#endif
