#ifndef SNACKIS_DB_COL_HPP
#define SNACKIS_DB_COL_HPP

#include <functional>
#include <string>

#include "snackis/core/type.hpp"
#include "snackis/db/table_col.hpp"

namespace snackis {
namespace db {
  template <typename RecT, typename ValT>
  struct Col: public TableCol<RecT> {
    const std::string name;
    const Type<ValT> &type;
    std::function<const ValT &(const RecT &)> get;
    std::function<void (RecT &, const ValT &)> set;

    Col(const std::string &name, const Type<ValT> &type, ValT RecT::* ptr);
    void copy(Rec<RecT> &dest, const RecT &src) const override;
  };

  template <typename RecT, typename ValT>
  Col<RecT, ValT>::Col(const std::string &name,
			     const Type<ValT> &type,
			     ValT RecT::* ptr):
    TableCol<RecT>(name),
    type(type),
    get([ptr](const RecT &rec) { return std::cref(rec.*ptr); }),
    set([ptr](RecT &rec, const ValT &val) { rec.*ptr = val; }) {
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::copy(Rec<RecT> &dest, const RecT &src) const {
    dest[this] = get(src);
  }
}}

#endif
