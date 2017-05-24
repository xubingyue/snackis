#ifndef SNACKIS_DB_BASIC_COL_HPP
#define SNACKIS_DB_BASIC_COL_HPP

#include <iostream>

#include "snackis/core/str.hpp"
#include "snackis/db/rec.hpp"

namespace snackis {
namespace db {
  template <typename RecT>
  struct BasicCol {
    const str name;
    BasicCol(const str &name);
    virtual void copy(Rec<RecT> &dest, const RecT &src) const=0;
    virtual void copy(RecT &dest, const Rec<RecT> &src) const=0;
    virtual void set(RecT &dest, const Val &val) const=0;
    virtual Val read(std::istream &in) const=0;
    virtual void write(const Val &val, std::ostream &out) const=0;
  };

  template <typename RecT>
  BasicCol<RecT>::BasicCol(const str &name): name(name) { }
}}

#endif
