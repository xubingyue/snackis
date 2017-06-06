#ifndef SNACKIS_DB_REC_HPP
#define SNACKIS_DB_REC_HPP

#include <map>
#include <string>

#include "snackis/core/val.hpp"

namespace snackis {
namespace db {
  template <typename RecT>
  struct BasicCol;

  template <typename RecT, typename ValT>
  struct Col;

  template <typename RecT>
  using Rec = std::map<const BasicCol<RecT> *, Val>;

  template <typename RecT, typename ValT>
  void set(Rec<RecT> &rec, const Col<RecT, ValT> &col, const ValT &val) {
    rec[&col] = val;
  }
}}

#endif
