#ifndef SNACKIS_TABLE_HPP
#define SNACKIS_TABLE_HPP

#include <fstream>
#include <set>
#include <string>

#include "snackis/db/context.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/record.hpp"

namespace snackis {
  template <typename RecT>
  struct Table: public Schema<RecT> {
    using CmpT = std::function<bool (const Record<RecT> &, const Record<RecT> &)>;

    Context &context;
    const std::string name;
    const Schema<RecT> key;
    std::set<Index *> indexes
    std::set<Record<RecT>, CmpT> records;
    std::ofstream file;
    
    Table(Context &ctx,
	  const std::string &name,
	  std::initializer_list<const TableColumn<RecT> *> key_cols,
	  std::initializer_list<const TableColumn<RecT> *> cols);
  };

  template <typename RecT>
  Table<RecT>::Table(Context &ctx,
		     const std::string &name,
		     std::initializer_list<const TableColumn<RecT> *> key_cols,
		     std::initializer_list<const TableColumn<RecT> *> cols):
    Schema<RecT>(cols),
    context(ctx),
    name(name),
    key(key_cols),
    records([this](const Record<RecT> &x, const Record<RecT> &y) {
	return compare(key, x, y) < 0;
      }) {
  }

  template <typename RecT>
  void open(Table<RecT> &tbl) {
    tbl.file.open(get_path(tbl.context, tbl.name + ".tbl"),
		  std::ios::out | std::ios::binary | std::ios::app);
  }

  template <typename RecT>
  void close(Table<RecT> &tbl) {
    tbl.file.close();
  }

  template <typename RecT>
  bool insert(Table<RecT> &tbl, const RecT &_rec) {
    Record<RecT> rec;
    copy(tbl, rec, _rec);
    return insert(tbl, rec);
  }

  template <typename RecT>
  bool insert(Table<RecT> &tbl, const Record<RecT> &rec) {
    auto found(tbl.records.find(rec));

    if (found == tbl.records.end()) {
      for (auto idx: tbl.indexes) {
	idx->insert(rec);
      }
      
      tbl.records.insert(rec);
      return true;
    }

    return false;
  }
}

#endif
