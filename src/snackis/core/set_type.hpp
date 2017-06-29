#ifndef SNACKIS_SET_TYPE_HPP
#define SNACKIS_SET_TYPE_HPP

#include <set>
#include "snackis/core/int64_type.hpp"
#include "snackis/core/stream.hpp"

namespace snackis {
  template <typename ValT>
  struct SetType: Type<std::set<ValT>> {
    Type<ValT> &val_type;
    
    SetType(Type<ValT> &val_type);
    bool is_null(const std::set<ValT> &val) const override;
    std::set<ValT> from_val(const Val &in) const override;
    Val to_val(const std::set<ValT> &in) const override;
    std::set<ValT> read(std::istream &in) const override;
    void write(const std::set<ValT> &val, std::ostream &out) const override;
  };

  template <typename ValT>
  SetType<ValT> set_type(Type<ValT> &val_type) { return SetType<ValT>(val_type); }
  
  template <typename ValT>
  SetType<ValT>::SetType(Type<ValT> &val_type):
    Type<std::set<ValT>>(fmt("Set(%0)", val_type.name)), val_type(val_type)
  { }

  template <typename ValT>
  bool SetType<ValT>::is_null(const std::set<ValT> &val) const {
    return val.empty();
  }

  template <typename ValT>
  std::set<ValT> SetType<ValT>::from_val(const Val &in) const {
    Stream out(get<str>(in));
    return read(out);
  }

  template <typename ValT>
  Val SetType<ValT>::to_val(const std::set<ValT> &in) const {
    Stream out;
    write(in, out);
    return out.str();
  }

  template <typename ValT>
  std::set<ValT> SetType<ValT>::read(std::istream &in) const {
    int64_t len(int64_type.read(in));
    std::set<ValT> out;
    for (size_t i = 0; i < len; i++) { out.insert(val_type.read(in)); }
    return out;
  }
  
  template <typename ValT>
  void SetType<ValT>::write(const std::set<ValT> &in,
			       std::ostream &out) const {
    int64_type.write(in.size(), out);
    for (const auto &v: in) { val_type.write(v, out); }
  }
}

#endif
