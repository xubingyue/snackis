#ifndef SNACKIS_DB_SCHEMA_HPP
#define SNACKIS_DB_SCHEMA_HPP

#include <initializer_list>
#include <vector>

#include <snackis/core/int64_type.hpp>
#include <snackis/core/str.hpp>
#include <snackis/core/str_type.hpp>
#include <snackis/crypt/secret.hpp>
#include <snackis/db/basic_col.hpp>
#include <snackis/db/rec.hpp>

namespace snackis {
namespace db {
  template <typename RecT>
  struct Schema {
    using Cols = std::initializer_list<const BasicCol<RecT> *>;
    std::vector<const BasicCol<RecT> *> cols;
    std::map<str, const BasicCol<RecT> *> col_lookup;
    Schema(Cols cols);
  };

  template <typename RecT>
  Schema<RecT>::Schema(Cols cols) {
    for (auto c: cols) { add(*this, *c); }
  }

  template <typename RecT>
  void add(Schema<RecT> &scm, const BasicCol<RecT> &col) {
    scm.cols.push_back(&col);
    scm.col_lookup[col.name] = &col;
  }

  template <typename RecT>
  int compare(const Schema<RecT> &scm, const RecT &x, const RecT &y) {
    for (auto c: scm.cols) {
      auto xv(c->get(x));
      auto yv(c->get(y));
      if (xv < yv) { return -1; }
      if (yv < xv) { return 1; }
    }

    return 0;
  }
  
  template <typename RecT>
  int compare(const Schema<RecT> &scm, const Rec<RecT> &x, const Rec<RecT> &y) {
    for (auto c: scm.cols) {
      auto xi = x.find(c);
      auto yi = y.find(c);

      if (xi == x.end() && yi == y.end()) { continue; }
      if (xi == x.end()) { return 1; }
      if (yi == y.end()) { return -1; }

      if (*xi < *yi) { return -1; }
      if (*yi < *xi) { return 1; }
    }

    return 0;
  }

  template <typename RecT>
  void copy(const Schema<RecT> &scm, RecT &dest, const RecT &src) {
    for (auto c: scm.cols) { c->copy(dest, src); }
  }

  template <typename RecT>
  void copy(const Schema<RecT> &scm, Rec<RecT> &dest, const RecT &src) {
    for (auto c: scm.cols) { c->copy(dest, src); }
  }

  template <typename RecT>
  void copy(const Schema<RecT> &scm, RecT &dest, const Rec<RecT> &src) {
    for (auto c: scm.cols) { c->copy(dest, src); }
  }

  template <typename RecT>
  void copy(const Schema<RecT> &scm, Rec<RecT> &dest, const Rec<RecT> &src) {
    for (auto c: scm.cols) {
      auto found = src.find(c);
      if (found != src.end()) { dest[c] = found->second; }
    }
  }

  template <typename RecT>
  void read(const Schema<RecT> &scm,
	    std::istream &in,
	    Rec<RecT> &rec,
	    opt<crypt::Secret> sec) {
    if (sec) {
      int64_t size(int64_type.read(in));
      Data edata(size);
      in.read((char *)&edata[0], size);
      const Data ddata(decrypt(*sec, (unsigned char *)&edata[0], size));
      Stream buf(str(ddata.begin(), ddata.end()));
      read(scm, buf, rec, nullopt);
    } else {
      int64_t cnt(int64_type.read(in));

      for (int64_t i=0; i<cnt; i++) {
	const str cname(str_type.read(in));
	auto found(scm.col_lookup.find(cname));
	
	if (found != scm.col_lookup.end()) {
	  auto c = found->second;
	  rec[c] = c->read(in);
	}
      }
    }
  }
  
  template <typename RecT>
  void write(const Schema<RecT> &scm,
	     const Rec<RecT> &rec,
	     std::ostream &out,
	     opt<crypt::Secret> sec) {
    if (sec) {
	Stream buf;
	write(scm, rec, buf, nullopt);
	str data(buf.str());
	const Data edata(encrypt(*sec, (unsigned char *)data.c_str(), data.size()));
	int64_type.write(edata.size(), out);
	out.write((char *)&edata[0], edata.size());
    } else {
      int64_t cnt(0);

      for (auto c: scm.cols) {
	auto found = rec.find(c);
	if (found != rec.end()) { cnt++; }
      }

      int64_type.write(cnt, out);
    
      for (auto c: scm.cols) {
	auto found = rec.find(c);
	
	if (found != rec.end()) {
	  str_type.write(c->name, out);
	  c->write(rec.at(c), out);
	}
      }
    }
  }
}}

#endif
