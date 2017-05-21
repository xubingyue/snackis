#ifndef SNACKIS_SCHEMA_HPP
#define SNACKIS_SCHEMA_HPP

#include <initializer_list>
#include <vector>

#include <snackis/db/record.hpp>

namespace snackis {
  template <typename RecT>
  struct Schema {
    std::vector<const TableColumn<RecT> *> columns;

    Schema(std::initializer_list<const TableColumn<RecT> *> cols);
  };

  template <typename RecT>
  Schema<RecT>::Schema(std::initializer_list<const TableColumn<RecT> *> cols):
    columns(cols) {
  }

  template <typename RecT>
  int compare(const Schema<RecT> &scm,
	      const Record<RecT> &x, const Record<RecT> &y) {
    for (auto c: scm.columns) {
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
  void copy(const Schema<RecT> scm, Record<RecT> dest, const RecT &src) {
    for (auto c: scm.columns) {
      c->copy(dest, src);
    }
  }
}

#endif
