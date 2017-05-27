#ifndef SNACKIS_DB_COL_HPP
#define SNACKIS_DB_COL_HPP

#include <functional>

#include "snackis/core/str.hpp"
#include "snackis/core/type.hpp"
#include "snackis/core/val.hpp"
#include "snackis/db/basic_col.hpp"

namespace snackis {
namespace db {
  template <typename RecT, typename ValT>
  struct Col: public BasicCol<RecT> {
    const Type<ValT> &type;
    std::function<const ValT &(const RecT &)> getter;
    std::function<void (RecT &, const ValT &)> setter;

    template <typename FldT>
    Col(const str &name, const Type<ValT> &type, FldT RecT::* ptr);
    void copy(RecT &dest, const RecT &src) const override;
    void copy(Rec<RecT> &dest, const RecT &src) const override;
    void copy(RecT &dest, const Rec<RecT> &src) const override;
    void set(RecT &dest, const Val &val) const override;
    Val read(std::istream &in) const override;
    void write(const Val &val, std::ostream &out) const override;
  };

  template <typename RecT, typename ValT>
  template <typename FldT>
  Col<RecT, ValT>::Col(const str &name,
			     const Type<ValT> &type,
			     FldT RecT::* ptr):
    BasicCol<RecT>(name),
    type(type),
    getter([ptr](const RecT &rec) { return std::cref(rec.*ptr); }),
    setter([ptr](RecT &rec, const ValT &val) { rec.*ptr = static_cast<FldT>(val); })
  { }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::copy(RecT &dest, const RecT &src) const {
    setter(dest, getter(src));
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::copy(Rec<RecT> &dest, const RecT &src) const {
    dest[this] = getter(src);
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::copy(RecT &dest, const Rec<RecT> &src) const {
    set(dest, snackis::get<ValT>(src.at(this)));
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::set(RecT &dest, const Val &val) const {
    setter(dest, snackis::get<ValT>(val));
  }

  template <typename RecT, typename ValT>
  Val Col<RecT, ValT>::read(std::istream &in) const {
    return type.read(in);
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::write(const Val &val, std::ostream &out) const {
    type.write(snackis::get<ValT>(val), out);
  }
}}

#endif
