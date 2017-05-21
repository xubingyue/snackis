#ifndef SNACKIS_TABLE_HPP
#define SNACKIS_TABLE_HPP

#include <fstream>
#include <set>
#include <string>

#include "snackis/db/context.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/record.hpp"

namespace snackis {
  using namespace std;
		       
  template <typename RecT>
  struct Table: public Schema<RecT> {
    using CmpT = function<bool (const Record<RecT> &, const Record<RecT> &)>;

    Context &context;
    const string name;
    const Schema<RecT> key;
    set<Record<RecT>, CmpT> records;
    ofstream file;
    
    Table(Context &ctx,
	  const string &name,
	  initializer_list<const TableColumn<RecT> *> key_cols,
	  initializer_list<const TableColumn<RecT> *> cols);
  };

  template <typename RecT>
  Table<RecT>::Table(Context &ctx,
		     const string &name,
		     initializer_list<const TableColumn<RecT> *> key_cols,
		     initializer_list<const TableColumn<RecT> *> cols):
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
		  ios::out | ios::binary | ios::app);
  }

  template <typename RecT>
  void close(Table<RecT> &tbl) {
    tbl.file.close();
  }
}

#endif
