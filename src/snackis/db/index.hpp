#ifndef SNACKIS_DB_INDEX_HPP
#define SNACKIS_DB_INDEX_HPP

#include "snackis/db/basic_table.hpp"
#include "snackis/db/rec.hpp"
#include "snackis/db/schema.hpp"

namespace snackis {  
namespace db {
  template <typename RecT>
  struct Index: BasicTable, Schema<RecT> {
    Index(Ctx &ctx, const str &name, const Schema<RecT> &cols);

    virtual bool insert(const Rec<RecT> &rec) = 0;
    virtual bool update(const Rec<RecT> &rec) = 0;
    virtual bool erase(const Rec<RecT> &rec) = 0;
  };

  template <typename RecT>
  Index<RecT>::Index(Ctx &ctx, const str &name, const Schema<RecT> &cols):
    BasicTable(ctx, name),
    Schema<RecT>(cols)
  { }

  template <typename RecT>
  bool insert(Index<RecT> &idx, const Rec<RecT> &rec) {
    return idx.insert(rec);
  }

  template <typename RecT>
  bool update(Index<RecT> &idx, const Rec<RecT> &rec) {
    return idx.update(rec);
  }

  template <typename RecT>
  bool erase(Index<RecT> &idx, const Rec<RecT> &rec) {
    return idx.erase(rec);
  }  
}}

#endif
