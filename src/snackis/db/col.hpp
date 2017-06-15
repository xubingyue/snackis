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
  struct Col: BasicCol<RecT> {
    Type<ValT> &type;
    ValT RecT::* field;
    
    Col(const str &name, Type<ValT> &type, ValT RecT::* fld);
    void copy(RecT &dest, const RecT &src) const override;
    void copy(Rec<RecT> &dest, const RecT &src) const override;
    void copy(RecT &dest, const Rec<RecT> &src) const override;
    void set(RecT &dest, const Val &val) const override;
    Val read(std::istream &in) const override;
    void write(const Val &val, std::ostream &out) const override;
  };

  template <typename RecT, typename ValT>
  Col<RecT, ValT>::Col(const str &name, Type<ValT> &type, ValT RecT::* fld):
    BasicCol<RecT>(name), type(type), field(fld)
  { }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::copy(RecT &dest, const RecT &src) const {
    dest.*field = src.*field;
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::copy(Rec<RecT> &dest, const RecT &src) const {
    dest[this] = type.to_val(src.*field);
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::copy(RecT &dest, const Rec<RecT> &src) const {
    set(dest, src.at(this));
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::set(RecT &rec, const Val &val) const {
    rec.*field = type.from_val(val);
  }

  template <typename RecT, typename ValT>
  Val Col<RecT, ValT>::read(std::istream &in) const {
    return type.read_val(in);
  }

  template <typename RecT, typename ValT>
  void Col<RecT, ValT>::write(const Val &val, std::ostream &out) const {
    type.write_val(val, out);
  }
}}

#endif
