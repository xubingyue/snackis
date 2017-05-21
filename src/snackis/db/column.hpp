#ifndef SNACKIS_COLUMN_HPP
#define SNACKIS_COLUMN_HPP

#include <functional>
#include <string>

#include "snackis/db/table_column.hpp"

namespace snackis {
  using namespace std;
  
  template <typename RecT, typename ValT>
  struct Column: public TableColumn<RecT> {
    const string name;
    const Type<ValT> &type;
    function<const ValT &(const RecT &)> get;
    function<void (RecT &, const ValT &)> set;

    Column(const string &name, const Type<ValT> &type, ValT RecT::* ptr);
  };

  template <typename RecT, typename ValT>
  Column<RecT, ValT>::Column(const string &name,
			     const Type<ValT> &type,
			     ValT RecT::* ptr):
    TableColumn<RecT>(name),
    type(type),
    get([ptr](const RecT &rec) { return cref(rec.*ptr); }),
    set([ptr](RecT &rec, const ValT &val) { rec.*ptr = val; }) {
  }
}

#endif
