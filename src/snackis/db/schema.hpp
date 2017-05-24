#ifndef SNACKIS_DB_SCHEMA_HPP
#define SNACKIS_DB_SCHEMA_HPP

#include <initializer_list>
#include <vector>

#include <snackis/core/str.hpp>
#include <snackis/db/rec.hpp>

namespace snackis {
namespace db {
  template <typename RecT>
  struct Schema {
    using Cols = std::initializer_list<const TableCol<RecT> *>;
    std::vector<const TableCol<RecT> *> cols;
    std::map<str, const TableCol<RecT> *> col_lookup;
    Schema(Cols cols);
  };

  template <typename RecT>
  Schema<RecT>::Schema(Cols cols) {
    for (auto c: cols) { add(*this, *c); }
  }

  template <typename RecT>
  void add(Schema<RecT> &scm, const TableCol<RecT> &col) {
    scm.cols.push_back(&col);
    scm.col_lookup[col.name] = &col;
  }
  
  template <typename RecT>
  int compare(const Schema<RecT> &scm, const Rec<RecT> &x, const Rec<RecT> &y) {
    for (auto c: scm.cols) {
      auto xi = x.find(c);
      auto yi = y.find(c);

      if (xi == x.end() && yi == y.end()) { continue; }
      if (xi == x.end()) { return -1; }
      if (yi == y.end()) { return 1; }

      if (*xi < *yi) { return -1; }
      if (*yi < *xi) { return 1; }
    }

    return 0;
  }

  template <typename RecT>
  void copy(const Schema<RecT> &scm, Rec<RecT> &dest, const RecT &src) {
    for (auto c: scm.cols) { c->copy(dest, src); }
  }

  template <typename RecT>
  void copy(const Schema<RecT> &scm, RecT &dest, const Rec<RecT> &src) {
    for (auto f: src) { f.first->set(dest, f.second); }
  }
}}

#endif
