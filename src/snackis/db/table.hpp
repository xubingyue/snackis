#ifndef SNACKIS_DB_TABLE_HPP
#define SNACKIS_DB_TABLE_HPP

#include <cstdint>
#include <set>

#include "snackis/core/data.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/core/func.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/type.hpp"
#include "snackis/core/stream.hpp"
#include "snackis/crypt/secret.hpp"
#include "snackis/db/basic_table.hpp"
#include "snackis/db/change.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/db/rec.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {  
namespace db {
  template <typename RecT>
  struct Table;

  template <typename RecT>
  struct RecType: public Type<Rec<RecT>> {
    Table<RecT> &table;    
    RecType(Table<RecT> &tbl);
    bool is_null(const Rec<RecT> &val) const override;
    Rec<RecT> from_val(const Val &in) const override;
    Val to_val(const Rec<RecT> &in) const override;
    Rec<RecT> read(std::istream &in) const override;
    void write(const Rec<RecT> &val, std::ostream &out) const override;
  };
  
  template <typename RecT>
  struct Table: BasicTable, Schema<RecT> {
    using CmpRec = func<bool (const Rec<RecT> &, const Rec<RecT> &)>;
    using Cols = std::initializer_list<const BasicCol<RecT> *>;
    using OnInsert = func<void (Rec<RecT> &)>;
    using OnUpdate = func<void (const Rec<RecT> &, Rec<RecT> &)>;
      
    const Schema<RecT> key;
    RecType<RecT> rec_type;
    std::set<Table<RecT> *> indexes;
    std::set<Rec<RecT>, CmpRec> recs;
    std::vector<OnInsert> on_insert;
    std::vector<OnUpdate> on_update;
    
    Table(Ctx &ctx, const str &name, Cols key_cols, Cols cols);
    virtual ~Table();
    void dump(std::ostream &out) override;
    void slurp() override;
  };
    
  enum TableOp {TABLE_INSERT, TABLE_UPDATE, TABLE_ERASE};

  template <typename RecT>
  struct TableChange: Change {
    TableOp op;
    Table<RecT> &table;
    const Rec<RecT> rec;

    TableChange(TableOp op, Table<RecT> &table, const Rec<RecT> &rec);
    Path table_path() const override;
    void write(std::ostream &out) const override;
  };

  template <typename RecT>
  struct Insert: TableChange<RecT> {
    Insert(Table<RecT> &table, const Rec<RecT> &rec);    
    void rollback() const override;
    void undo() const override;
  };

  template <typename RecT>
  struct Update: TableChange<RecT> {
    const Rec<RecT> prev_rec;
    Update(Table<RecT> &table, const Rec<RecT> &rec, const Rec<RecT> &prev_rec);    
    void rollback() const override;
    void undo() const override;
  };

  template <typename RecT>
  struct Erase: TableChange<RecT> {
    Erase(Table<RecT> &table, const Rec<RecT> &rec);    
    void rollback() const override;
    void undo() const override;
  };

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
  const Rec<RecT> *find(Table<RecT> &tbl, const Rec<RecT> &key) {
    auto found = tbl.recs.find(key);
    if (found == tbl.recs.end()) { return nullptr; }
    return &*found;
  }

  template <typename RecT>
  const Rec<RecT> &get(Table<RecT> &tbl, const Rec<RecT> &key) {
    auto found = tbl.recs.find(key);
    CHECK(found, _ != tbl.recs.end());
    return *found;
  }

  template <typename RecT>
  bool insert(Table<RecT> &tbl, const RecT &rec) {
    Rec<RecT> trec;
    copy(tbl, trec, rec);
    return insert(tbl, trec);
  }

  template <typename RecT>
  bool insert(Table<RecT> &tbl, const Rec<RecT> &rec) {
    TRACE(fmt("Inserting into table: %0", tbl.name));
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

    db::Rec<RecT> trec(rec);
    for (auto e: tbl.on_insert) { e(trec); }
    tbl.recs.insert(trec);
    log_change(get_trans(tbl.ctx), new Insert<RecT>(tbl, trec));
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
    TRACE(fmt("Updating table: %0", tbl.name));
    auto found(tbl.recs.find(rec));
    if (found == tbl.recs.end() || rec == *found) { return false; }
    
    for (auto idx: tbl.indexes) {
      erase(*idx, *found);
      
      Rec<RecT> irec;
      copy(idx->key, irec, rec);
      if (!irec.empty()) {
	copy(tbl.key, irec, rec);
	insert(*idx, irec);
      }
    }
    
    db::Rec<RecT> trec(rec);
    for (auto e: tbl.on_update) { e(*found, trec); }
    log_change(get_trans(tbl.ctx), new Update<RecT>(tbl, trec, *found));
    tbl.recs.erase(found);
    tbl.recs.insert(trec);
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
    TRACE(fmt("Erasing from table: %0", tbl.name));
    auto found(tbl.recs.find(rec));
    if (found == tbl.recs.end()) { return false; }
    log_change(get_trans(tbl.ctx), new Erase<RecT>(tbl, *found));
    for (auto idx: tbl.indexes) { erase(*idx, *found); }
    tbl.recs.erase(found);
    return true;
  }

  template <typename RecT>
  void read(const Table<RecT> &tbl,
	    std::istream &in,
	    Rec<RecT> &rec,
	    opt<crypt::Secret> sec) {
    if (sec) {
      int64_t size(int64_type.read(in));
      Data edata(size);
      in.read((char *)&edata[0], size);
      const Data ddata(decrypt(*sec, (unsigned char *)&edata[0], size));
      Stream buf(str(ddata.begin(), ddata.end()));
      read(tbl, buf, rec, nullopt);
    } else {
      int64_t cnt(int64_type.read(in));

      for (int64_t i=0; i<cnt; i++) {
	const str cname(str_type.read(in));
	auto found(tbl.col_lookup.find(cname));
	
	if (found != tbl.col_lookup.end()) {
	  auto c = found->second;
	  rec[c] = c->read(in);
	}
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
	int64_type.write(edata.size(), out);
	out.write((char *)&edata[0], edata.size());
    } else {
      int64_t cnt(0);

      for (auto c: tbl.cols) {
	auto found = rec.find(c);
	if (found != rec.end()) { cnt++; }
      }

      int64_type.write(cnt, out);
    
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
  void write(Table<RecT> &tbl, TableOp _op, const Rec<RecT> &rec, std::ostream &out) {
    uint8_t op(_op);
    out.write(reinterpret_cast<const char *>(&op), sizeof op);
    write(tbl, rec, out, tbl.ctx.secret);
  }

  template <typename RecT>
  void dump(Table<RecT> &tbl, std::ostream &out) {    
    for (auto &rec: tbl.recs) {
      write(tbl, TABLE_INSERT, rec, out);
    }
  }

  template <typename RecT>
  void slurp(Table<RecT> &tbl, std::istream &in) {    
    while (true) {
      uint8_t op;
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
    std::fstream f;
    
    f.open(get_path(tbl.ctx, tbl.name + ".tbl").string(),
	      std::ios::in | std::ios::binary);
    if (f.fail()) {
      ERROR(Db, fmt("Failed opening file: %0", tbl.name));
      return;
    }

    slurp(tbl, f);
    f.close();
  }
  
  template <typename RecT>
  RecType<RecT>::RecType(Table<RecT> &tbl):
    Type<Rec<RecT>>(fmt("Rec(%0)", tbl.name)), table(tbl) { }

  template <typename RecT>
  bool RecType<RecT>::is_null(const Rec<RecT> &val) const {
    return val.empty();
  }

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
    ctx.tables.erase(this);
  }

  template <typename RecT>
  void Table<RecT>::dump(std::ostream &out) { db::dump(*this, out); }

  template <typename RecT>
  void Table<RecT>::slurp() { db::slurp(*this); }

  template <typename RecT>
  TableChange<RecT>::TableChange(TableOp op,
				 Table<RecT> &table,
				 const Rec<RecT> &rec):
    op(op), table(table), rec(rec) { }

  template <typename RecT>
  Path TableChange<RecT>::table_path() const {
    return table.path;
  }

  template <typename RecT>
  void TableChange<RecT>::write(std::ostream &out) const {
    db::write(this->table, this->op, this->rec, out);
  }

  template <typename RecT>
  Insert<RecT>::Insert(Table<RecT> &table, const Rec<RecT> &rec):
    TableChange<RecT>(TABLE_INSERT, table, rec) { }
  
  template <typename RecT>
  void Insert<RecT>::rollback() const {
    this->table.recs.erase(this->rec);
  }

  template <typename RecT>
  void Insert<RecT>::undo() const {
    erase(this->table, this->rec);
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
  void Update<RecT>::undo() const {
    update(this->table, this->prev_rec);
  }

  template <typename RecT>
  Erase<RecT>::Erase(Table<RecT> &table, const Rec<RecT> &rec):
    TableChange<RecT>(TABLE_ERASE, table, rec) { }
  
  template <typename RecT>
  void Erase<RecT>::rollback() const {
    this->table.recs.insert(this->rec);
  }

  template <typename RecT>
  void Erase<RecT>::undo() const {
    insert(this->table, this->rec);
  }
}}

#endif
