#ifndef SNACKIS_TABLE_HPP
#define SNACKIS_TABLE_HPP

#include <cstdint>
#include <fstream>
#include <set>
#include <string>

#include "snackis/db/ctx.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/rec.hpp"

namespace snackis {  
  template <typename RecT>
  struct Table: public Schema<RecT> {
    using CmpRec = std::function<bool (const Rec<RecT> &, const Rec<RecT> &)>;
    using Cols = std::initializer_list<const TableCol<RecT> *>;

    Ctx &ctx;
    const std::string name;
    const Schema<RecT> key;
    std::set<Table<RecT> *> indexes;
    std::set<Rec<RecT>, CmpRec> recs;
    std::ofstream file;
    
    Table(Ctx &ctx, const std::string &name, Cols key_cols, Cols cols);
  };

  enum TableOp {TABLE_ERASE, TABLE_INSERT, TABLE_UPDATE};

  template <typename RecT>
  struct TableChange: public Change {
    TableOp op;
    Table<RecT> &table;
    const Rec<RecT> rec;

    TableChange(TableOp op, Table<RecT> &table, const Rec<RecT> &rec);
    void commit() const override;
  };

  template <typename RecT>
  struct Insert: public TableChange<RecT> {
    Insert(Table<RecT> &table, const Rec<RecT> &rec);    
    void rollback() const override;
  };

  template <typename RecT>
  Table<RecT>::Table(Ctx &ctx, const std::string &name, Cols key_cols, Cols cols):
    Schema<RecT>(cols),
    ctx(ctx),
    name(name),
    key(key_cols),
    recs([this](const Rec<RecT> &x, const Rec<RecT> &y) {
	return compare(key, x, y) < 0;
      }) { }

  template <typename RecT>
  void open(Table<RecT> &tbl) {
    tbl.file.open(get_path(tbl.ctx, tbl.name + ".tbl"),
		  std::ios::out | std::ios::binary | std::ios::app);
  }

  template <typename RecT>
  void close(Table<RecT> &tbl) {
    tbl.file.close();
  }

  template <typename RecT>
  bool insert(Table<RecT> &tbl, const RecT &_rec) {
    Rec<RecT> rec;
    copy(tbl, rec, _rec);
    return insert(tbl, rec);
  }

  template <typename RecT>
  bool insert(Table<RecT> &tbl, const Rec<RecT> &rec) {
    auto found(tbl.recs.find(rec));

    if (found == tbl.recs.end()) {
      for (auto idx: tbl.indexes) {
	insert(*idx, rec);
      }

      log_change(*tbl.ctx.trans, new Insert<RecT>(tbl, rec));
      tbl.recs.insert(rec);
      return true;
    }

    return false;
  }

  template <typename RecT>
  void write(Table<RecT> &tbl, const Rec<RecT> &rec) {
    //TODO: Write rec
  }
  
  template <typename RecT>
  TableChange<RecT>::TableChange(TableOp op,
				 Table<RecT> &table,
				 const Rec<RecT> &rec):
    op(op), table(table), rec(rec) { }

  template <typename RecT>
  void TableChange<RecT>::commit() const {
    this->table.file << int8_t(this->op);
    write(this->table, this->rec);
  }

  template <typename RecT>
  Insert<RecT>::Insert(Table<RecT> &table, const Rec<RecT> &rec):
    TableChange<RecT>(TABLE_INSERT, table, rec) { }
  
  template <typename RecT>
  void Insert<RecT>::rollback() const {
    this->table.recs.erase(this->rec);
  }
}

#endif
