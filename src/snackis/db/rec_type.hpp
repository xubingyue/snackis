#ifndef SNACKIS_DB_REC_TYPE_HPP
#define SNACKIS_DB_REC_TYPE_HPP

#include "snackis/db/schema.hpp"

namespace snackis {
namespace db {
  template <typename RecT>
  struct RecType: public Type<Rec<RecT>> {
    Schema<RecT> &schema;    
    RecType(Schema<RecT> &scm);
    bool is_null(const Rec<RecT> &val) const override;
    Rec<RecT> from_val(const Val &in) const override;
    Val to_val(const Rec<RecT> &in) const override;
    Rec<RecT> read(std::istream &in) const override;
    void write(const Rec<RecT> &val, std::ostream &out) const override;
  };

  template <typename RecT>
  RecType<RecT>::RecType(Schema<RecT> &scm):
    Type<Rec<RecT>>("Rec"), schema(scm) { }

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
    db::read(schema, in, rec, nullopt);
    return rec;
  }
  
  template <typename RecT>
  void RecType<RecT>::write(const Rec<RecT> &val, std::ostream &out) const {
    db::write(schema, val, out, nullopt);
  }
}}

#endif
