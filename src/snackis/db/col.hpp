#ifndef SNACKIS_DB_COL_HPP
#define SNACKIS_DB_COL_HPP

#include <functional>
#include <string>

#include "snackis/core/type.hpp"
#include "snackis/core/val.hpp"
#include "snackis/db/table_col.hpp"

namespace snackis {
namespace db {
  template <typename RecT, typename ValT>
  struct Col: public TableCol<RecT> {
    const std::string name;
    const Type<ValT> &type;
    std::function<const ValT &(const RecT &)> get;
    std::function<void (RecT &, const ValT &)> set;

    template <typename FldT>
    Col(const std::string &name, const Type<ValT> &type, FldT RecT::* ptr);
    void copy(Rec<RecT> &dest, const RecT &src) const override;
    void copy(RecT &dest, const Rec<RecT> &src) const override;
  };

  template <typename RecT, typename ValT>
  template <typename FldT>
  Col<RecT, ValT>::Col(const std::string &name,
			     const Type<ValT> &type,
			     FldT RecT::* ptr):
    TableCol<RecT>(name),
    type(type),
    get([ptr](const RecT &rec) { return std::cref(rec.*ptr); }),
    set([ptr](RecT &rec, const ValT &val) { rec.*ptr = static_cast<FldT>(val); }) {
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::copy(Rec<RecT> &dest, const RecT &src) const {
    dest[this] = get(src);
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::copy(RecT &dest, const Rec<RecT> &src) const {
    set(dest, snackis::get<ValT>(src.at(this)));
  }
}}

#endif
