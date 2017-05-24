#ifndef SNACKIS_DB_TABLE_COL_HPP
#define SNACKIS_DB_TABLE_COL_HPP

#include <iostream>
#include <string>
#include "snackis/db/rec.hpp"

namespace snackis {
namespace db {
  template <typename RecT>
  struct TableCol {
    const std::string name;
    TableCol(const std::string &name);
    virtual void copy(Rec<RecT> &dest, const RecT &src) const=0;
    virtual void copy(RecT &dest, const Rec<RecT> &src) const=0;
    virtual void set(RecT &dest, const Val &val) const=0;
    virtual Val read(std::istream &in) const=0;
    virtual void write(const Val &val, std::ostream &out) const=0;
  };

  template <typename RecT>
  TableCol<RecT>::TableCol(const std::string &name): name(name) { }
}}

#endif
