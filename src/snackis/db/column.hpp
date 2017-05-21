#ifndef SNACKIS_COLUMN_HPP
#define SNACKIS_COLUMN_HPP

#include <functional>
#include <string>

#include "snackis/db/table_column.hpp"

namespace snackis {
  template <typename RecT, typename ValT>
  struct Column: public TableColumn<RecT> {
    const std::string name;
    const Type<ValT> &type;
    std::function<const ValT &(const RecT &)> get;
    std::function<void (RecT &, const ValT &)> set;

    Column(const std::string &name, const Type<ValT> &type, ValT RecT::* ptr);
    void copy(Record<RecT> &dest, const RecT &src) const override;
  };

  template <typename RecT, typename ValT>
  Column<RecT, ValT>::Column(const std::string &name,
			     const Type<ValT> &type,
			     ValT RecT::* ptr):
    TableColumn<RecT>(name),
    type(type),
    get([ptr](const RecT &rec) { return std::cref(rec.*ptr); }),
    set([ptr](RecT &rec, const ValT &val) { rec.*ptr = val; }) {
  }

  template <typename RecT, typename ValT>
  void Column<RecT, ValT>::copy(Record<RecT> &dest, const RecT &src) const {
    dest[this] = get(src);
  }
}

#endif
