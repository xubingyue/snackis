#ifndef SNACKIS_SCHEMA_HPP
#define SNACKIS_SCHEMA_HPP

#include <initializer_list>
#include <vector>

#include <snackis/db/rec.hpp>

namespace snackis {
  template <typename RecT>
  struct Schema {
    using Cols = std::initializer_list<const TableCol<RecT> *>;
    std::vector<const TableCol<RecT> *> cols;
    Schema(Cols cols);
  };

  template <typename RecT>
  Schema<RecT>::Schema(Cols cols): cols(cols) { }

  template <typename RecT>
  int compare(const Schema<RecT> &scm, const Rec<RecT> &x, const Rec<RecT> &y) {
    for (auto c: scm.cols) {
      auto xi = x.find(c);
      auto yi = y.find(c);

      if (xi == x.end() && yi == y.end()) {
	continue;
      }

      if (xi == x.end()) {
	return -1;
      }

      if (yi == y.end()) {
	return 1;
      }

      if (*xi < *yi) {
	return -1;
      }

      if (*yi < *xi) {
	return 1;
      }
    }

    return 0;
  }

  template <typename RecT>
  void copy(const Schema<RecT> scm, Rec<RecT> dest, const RecT &src) {
    for (auto c: scm.cols) {
      c->copy(dest, src);
    }
  }
}

#endif
