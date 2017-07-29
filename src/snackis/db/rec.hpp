#ifndef SNACKIS_DB_REC_HPP
#define SNACKIS_DB_REC_HPP

#include <map>
#include <string>

#include "snackis/core/int64_type.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/val.hpp"
#include "snackis/crypt/secret.hpp"

namespace snackis {
namespace db {
  template <typename RecT>
  struct BasicCol;

  template <typename RecT, typename ValT>
  struct Col;

  template <typename RecT>
  struct Schema;

  template <typename RecT>
  struct Rec: std::map<const BasicCol<RecT> *, Val> {
    Rec();
    Rec(const Schema<RecT> &scm, const RecT &src);
    Rec(const Schema<RecT> &scm, const Rec<RecT> &src);
  };

  template <typename RecT>
  Rec<RecT>::Rec()
  { }

  template <typename RecT>
  Rec<RecT>::Rec(const Schema<RecT> &scm, const RecT &src) {
    copy(scm, *this, src);
  }

  template <typename RecT>
  Rec<RecT>::Rec(const Schema<RecT> &scm, const Rec<RecT> &src) {
    copy(scm, *this, src);
  }

  template <typename RecT, typename ValT>
  opt<ValT> get(const Rec<RecT> &rec, const Col<RecT, ValT> &col) {
    auto found(rec.find(&col));
    return (found == rec.end()) ? nullopt : opt<ValT>(get<ValT>(found->second));
  }

  template <typename RecT, typename ValT>
  void set(Rec<RecT> &rec, const Col<RecT, ValT> &col, const ValT &val) {
    rec[&col] = val;
  }

  template <typename RecT>
  void copy(RecT &dest, const db::Rec<RecT> &src) {
    for (auto &f: src) { f.first->set(dest, f.second); }
  }

  template <typename RecT>
  void write(const Rec<RecT> &rec,
	     std::ostream &out,
	     opt<crypt::Secret> sec) {
    if (sec) {
	Stream buf;
	write(rec, buf, nullopt);
	str data(buf.str());
	const Data edata(encrypt(*sec, (unsigned char *)data.c_str(), data.size()));
	int64_type.write(edata.size(), out);
	out.write((char *)&edata[0], edata.size());
    } else {
      int64_type.write(rec.size(), out);
    
      for (auto f: rec) {
	auto c(f.first);
	str_type.write(c->name, out);
	c->write(f.second, out);
      }
    }
  }
}}

#endif
