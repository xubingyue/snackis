#ifndef SNACKIS_DB_TABLE_HPP
#define SNACKIS_DB_TABLE_HPP

#include <cassert>
#include <cstdint>
#include <set>

#include "snackis/core/data.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/core/func.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/type.hpp"
#include "snackis/core/stream.hpp"
#include "snackis/crypt/secret.hpp"
#include "snackis/db/basic_table.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/rec.hpp"

namespace snackis {  
namespace db {
  template <typename RecT>
  struct Table;

  template <typename RecT>
  struct RecType: public Type<Rec<RecT>> {
    Table<RecT> &table;
    
    RecType(Table<RecT> &tbl);
    Rec<RecT> from_val(const Val &in) const override;
    Val to_val(const Rec<RecT> &in) const override;
    Rec<RecT> read(std::istream &in) const override;
    void write(const Rec<RecT> &val, std::ostream &out) const override;
  };
  
  template <typename RecT>
  struct Table: public BasicTable, public Schema<RecT> {
    using CmpRec = func<bool (const Rec<RecT> &, const Rec<RecT> &)>;
    using Cols = std::initializer_list<const BasicCol<RecT> *>;
    
    const Schema<RecT> key;
    RecType<RecT> rec_type;
    std::set<Table<RecT> *> indexes;
    std::set<Rec<RecT>, CmpRec> recs;
    
    Table(Ctx &ctx, const str &name, Cols key_cols, Cols cols);
    virtual ~Table();
    void slurp() override;
  };
    
  enum TableOp {TABLE_INSERT, TABLE_UPDATE, TABLE_ERASE};

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
  struct Update: public TableChange<RecT> {
    const Rec<RecT> prev_rec;
    Update(Table<RecT> &table, const Rec<RecT> &rec, const Rec<RecT> &prev_rec);    
    void rollback() const override;
  };

  template <typename RecT>
  struct Erase: public TableChange<RecT> {
    Erase(Table<RecT> &table, const Rec<RecT> &rec);    
    void rollback() const override;
  };

  template <typename RecT>
  void close(Table<RecT> &tbl) {
    tbl.file.close();
  }

  template <typename RecT>
  opt<RecT> load(Table<RecT> &tbl, RecT &rec) {
    Rec<RecT> key;
    copy(tbl.key, key, rec);
    auto found = tbl.recs.find(key);
    if (found == tbl.recs.end()) { return nullopt; }
    copy(tbl, rec, *found);
    return rec;
  }

  template <typename RecT>
  opt<Rec<RecT>> load(Table<RecT> &tbl, Rec<RecT> &rec) {
    auto found = tbl.recs.find(rec);
    if (found == tbl.recs.end()) { return nullopt; }
    copy(tbl, rec, *found);
    return rec;
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
    if (found != tbl.recs.end()) { return false; }

    for (auto idx: tbl.indexes) {
      Rec<RecT> irec;
      copy(idx->key, irec, rec);
      if (!irec.empty()) {
	copy(tbl.key, irec, rec);
	insert(*idx, irec);
      }
    }
    
    assert(tbl.ctx.trans);
    log_change(*tbl.ctx.trans, new Insert<RecT>(tbl, rec));
    tbl.recs.insert(rec);
    return true;
  }

  template <typename RecT>
  bool update(Table<RecT> &tbl, const RecT &rec) {
    Rec<RecT> trec;
    copy(tbl, trec, rec);
    return update(tbl, trec);
  }

  template <typename RecT>
  bool update(Table<RecT> &tbl, const Rec<RecT> &rec) {
    auto found(tbl.recs.find(rec));
    if (found == tbl.recs.end()) { return false; }
    
    for (auto idx: tbl.indexes) {
      erase(*idx, *found);
      
      Rec<RecT> irec;
      copy(idx->key, irec, rec);
      if (!irec.empty()) {
	copy(tbl.key, irec, rec);
	insert(*idx, irec);
      }
    }
    
    assert(tbl.ctx.trans);
    log_change(*tbl.ctx.trans, new Update<RecT>(tbl, rec, *found));
    tbl.recs.erase(found);
    tbl.recs.insert(rec);
    return true;
  }

  template <typename RecT>
  bool upsert(Table<RecT> &tbl, const RecT &rec) {
    if (insert(tbl, rec)) { return true; }
    update(tbl, rec);
    return false;
  }

  template <typename RecT>
  bool erase(Table<RecT> &tbl, const RecT &rec) {
    Rec<RecT> trec;
    copy(tbl.key, trec, rec);
    return erase(tbl, trec);
  }

  template <typename RecT>
  bool erase(Table<RecT> &tbl, const Rec<RecT> &rec) {
    auto found(tbl.recs.find(rec));
    if (found == tbl.recs.end()) { return false; }
    for (auto idx: tbl.indexes) { erase(*idx, *found); }
    assert(tbl.ctx.trans);
    log_change(*tbl.ctx.trans, new Erase<RecT>(tbl, *found));
    tbl.recs.erase(found);
    return true;
  }

  template <typename RecT>
  void read(const Table<RecT> &tbl,
	    std::istream &in,
	    Rec<RecT> &rec,
	    opt<crypt::Secret> sec) {
    if (sec) {
      int64_t size = -1;
      in.read((char *)&size, sizeof size);
      Data edata;
      edata.resize(size);
      in.read((char *)&edata[0], size);
      const Data ddata(decrypt(*sec, (unsigned char *)&edata[0], size));
      Stream buf(str(ddata.begin(), ddata.end()));
      read(tbl, buf, rec, nullopt);
    } else {
      int32_t cnt = -1;
      in.read((char *)&cnt, sizeof cnt);

      for (int32_t i=0; i<cnt; i++) {
	const str cname(str_type.read(in));
	auto found(tbl.col_lookup.find(cname));
	if (found == tbl.col_lookup.end()) {
	  ERROR(Db, fmt("Column not found: %0/%1", tbl.name, cname));
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
	     opt<crypt::Secret> sec) {
    if (sec) {
	Stream buf;
	write(tbl, rec, buf, nullopt);
	str data(buf.str());
	const Data edata(encrypt(*sec, (unsigned char *)data.c_str(), data.size()));
	const int64_t size = edata.size();
	out.write((char *)&size, sizeof size);
	out.write((char *)&edata[0], size);
    } else {
      int32_t cnt(0);

      for (auto c: tbl.cols) {
	auto found = rec.find(c);
	if (found != rec.end()) { cnt++; }
      }

      out.write((const char *)&cnt, sizeof cnt);
    
      for (auto c: tbl.cols) {
	auto found = rec.find(c);
	
	if (found != rec.end()) {
	  str_type.write(c->name, out);
	  c->write(rec.at(c), out);
	}
      }
    }
  }

  template <typename RecT>
  void write(Table<RecT> &tbl, const Rec<RecT> &rec, TableOp _op) {
    int8_t op(_op);
    tbl.file.write(reinterpret_cast<const char *>(&op), sizeof op);
    write(tbl, rec, tbl.file, tbl.ctx.secret);
    dirty_file(tbl.ctx, tbl.file);
  }

  template <typename RecT>
  void slurp(Table<RecT> &tbl, std::istream &in) {
    int8_t op(-1);
    
    while (true) {
      in.read(reinterpret_cast<char *>(&op), sizeof op);

      if (in.eof()) {
	in.clear();
	break;
      }

      if (in.fail()) {
	in.clear();
	ERROR(Db, fmt("Failed reading: %0", tbl.name));
      }
            
      Rec<RecT> rec;
      read(tbl, in, rec, tbl.ctx.secret);

      switch (op) {
      case TABLE_INSERT:
	tbl.recs.insert(rec);
	break;
      case TABLE_UPDATE:
	tbl.recs.erase(rec);
	tbl.recs.insert(rec);
	break;
      case TABLE_ERASE:
	tbl.recs.erase(rec);
	break;
      default:
	log(tbl.ctx, fmt("Invalid table operation: %0", op));
      }
    }
  }

  template <typename RecT>
  void slurp(Table<RecT> &tbl) {
    std::fstream file;
    
    file.open(get_path(tbl.ctx, tbl.name + ".tbl").string(),
	      std::ios::in | std::ios::binary);
    if (tbl.file.fail()) {
      ERROR(Db, fmt("Failed opening file: %0", tbl.name));
    }

    slurp(tbl, file);
    file.close();
  }

  template <typename RecT>
  Table<RecT>::Table(Ctx &ctx, const str &name, Cols key_cols, Cols cols):
    BasicTable(ctx, name),
    Schema<RecT>(cols),
    key(key_cols),
    rec_type(*this),
    recs([this](const Rec<RecT> &x, const Rec<RecT> &y) {
	return compare(key, x, y) < 0;
      }) {
    for (auto c: key.cols) { add(*this, *c); }
    ctx.tables.insert(this);
  }

  template <typename RecT>
  Table<RecT>::~Table() {
    if (file.is_open()) { close(*this); }
    ctx.tables.erase(this);
  }

  template <typename RecT>
  void Table<RecT>::slurp() { db::slurp(*this); }
  
  template <typename RecT>
  TableChange<RecT>::TableChange(TableOp op,
				 Table<RecT> &table,
				 const Rec<RecT> &rec):
    op(op), table(table), rec(rec) { }

  template <typename RecT>
  void TableChange<RecT>::commit() const {
    write(this->table, this->rec, this->op);
  }

  template <typename RecT>
  Insert<RecT>::Insert(Table<RecT> &table, const Rec<RecT> &rec):
    TableChange<RecT>(TABLE_INSERT, table, rec) { }
  
  template <typename RecT>
  void Insert<RecT>::rollback() const {
    this->table.recs.erase(this->rec);
  }

  template <typename RecT>
  Update<RecT>::Update(Table<RecT> &table,
		       const Rec<RecT> &rec, const Rec<RecT> &prev_rec):
    TableChange<RecT>(TABLE_UPDATE, table, rec), prev_rec(prev_rec) { }
  
  template <typename RecT>
  void Update<RecT>::rollback() const {
    this->table.recs.erase(this->rec);
    this->table.recs.insert(this->prev_rec);
  }

  template <typename RecT>
  Erase<RecT>::Erase(Table<RecT> &table, const Rec<RecT> &rec):
    TableChange<RecT>(TABLE_ERASE, table, rec) { }
  
  template <typename RecT>
  void Erase<RecT>::rollback() const {
    this->table.recs.insert(this->rec);
  }

  template <typename RecT>
  RecType<RecT>::RecType(Table<RecT> &tbl):
    Type<Rec<RecT>>(fmt("Rec(%0)", tbl.name)), table(tbl) { }

  template <typename RecT>
  Rec<RecT> RecType<RecT>::from_val(const Val &in) const {
    Stream out(get<str>(in));
    return read(out);
  }

  template <typename RecT>
  Val RecType<RecT>::to_val(const Rec<RecT> &in) const {
    Stream out;
    write(in, out);
    return out.str();
  }

  template <typename RecT>
  Rec<RecT> RecType<RecT>::read(std::istream &in) const {
    Rec<RecT> rec;
    db::read(table, in, rec, nullopt);
    return rec;
  }
  
  template <typename RecT>
  void RecType<RecT>::write(const Rec<RecT> &val, std::ostream &out) const {
    db::write(table, val, out, nullopt);
  }
}}

#endif
