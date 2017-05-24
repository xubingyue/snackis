#ifndef SNACKIS_DB_TABLE_HPP
#define SNACKIS_DB_TABLE_HPP

#include <cstdint>
#include <fstream>
#include <set>
#include <sstream>
#include <string>

#include "snackis/core/fmt.hpp"
#include "snackis/core/string_type.hpp"
#include "snackis/core/type.hpp"
#include "snackis/crypt/secret.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/rec.hpp"

namespace snackis {  
namespace db {
  template <typename RecT>
  struct Table;
  
  template <typename RecT>
  struct RecType: public Type<void *> {
    Table<RecT> &tbl;
    
    RecType(Table<RecT> &tbl);
    void *read(std::istream &in) const override;
    void write(void *const &val, std::ostream &out) const override;
  };
  
  template <typename RecT>
  struct Table: public Schema<RecT> {
    using CmpRec = std::function<bool (const Rec<RecT> &, const Rec<RecT> &)>;
    using Cols = std::initializer_list<const TableCol<RecT> *>;
    
    Ctx &ctx;
    const std::string name;
    const Schema<RecT> key;
    const RecType<RecT> rec_type;
    std::set<Table<RecT> *> indexes;
    std::set<Rec<RecT>, CmpRec> recs;
    std::ofstream file;
    const crypt::Secret *secret;
    
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
    rec_type(*this),
    recs([this](const Rec<RecT> &x, const Rec<RecT> &y) {
	return compare(key, x, y) < 0;
      }),
    secret(nullptr) {
    for (auto c: key.cols) { add(*this, *c); }
  }

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
  bool insert(Table<RecT> &tbl, const RecT &rec) {
    Rec<RecT> trec;
    copy(tbl, trec, rec);
    return insert(tbl, trec);
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
  void read(const Table<RecT> &tbl,
	    std::istream &in,
	    Rec<RecT> &rec,
	    const crypt::Secret *sec) {
    if (sec) {
      int64_t size = -1;
      in.read((char *)&size, sizeof size);
      std::vector<unsigned char> edata;
      edata.resize(size);
      in.read((char *)&edata[0], size);
      
      std::vector<unsigned char> ddata(decrypt(*sec,
					       (unsigned char *)&edata[0],
					       size));
      std::stringstream buf(std::string(ddata.begin(), ddata.end()));
      read(tbl, buf, rec, nullptr);
    } else {
      int32_t cnt = -1;
      in.read((char *)&cnt, sizeof cnt);

      for (int32_t i=0; i<cnt; i++) {
	const std::string cname(string_type.read(in));
	auto found(tbl.col_lookup.find(cname));
	if (found == tbl.col_lookup.end()) {
	  ERROR(Db, fmt("Column not found: %1%") % cname);
	}
	
	auto c = found->second;
	rec[c] = c->read(in);
      }
    }
  }

  template <typename RecT>
  void write(const Table<RecT> &tbl,
	     const Rec<RecT> &rec,
	     std::ostream &out,
	     const crypt::Secret *sec) {
    if (sec) {
	std::stringstream buf;
	write(tbl, rec, buf, nullptr);
	std::string data(buf.str());
	std::vector<unsigned char> edata(encrypt(*sec,
						 (unsigned char *)data.c_str(),
						 data.size()));
	const int64_t size = edata.size();
	out.write((char *)&size, sizeof size);
	out.write((char *)&edata[0], size);
    } else {
      const int32_t cnt(rec.size());
      out.write((const char *)&cnt, sizeof cnt);
    
      for (auto f: rec) {
	string_type.write(f.first->name, out);
	f.first->write(f.second, out);
      }
    }
  }

  template <typename RecT>
  void write(Table<RecT> &tbl, const Rec<RecT> &rec) {
    write(tbl, rec, tbl.file, tbl.secret);
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

  template <typename RecT>
  RecType<RecT>::RecType(Table<RecT> &tbl):
    Type(fmt("Rec(%1%)") % tbl.name), tbl(tbl) { }

  template <typename RecT>
  void *RecType<RecT>::read(std::istream &in) const {
    Rec<RecT> trec;
    db::read(tbl, in, trec, nullptr);
    return new RecT(tbl, trec);
  }
  
  template <typename RecT>
  void RecType<RecT>::write(void *const &val, std::ostream &out) const {
    Rec<RecT> trec;
    copy(tbl, trec, *static_cast<RecT *>(val));
    db::write(tbl, trec, out, nullptr);
  }
}}

#endif
