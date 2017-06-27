#ifndef SNACKIS_PRIM_TYPE_HPP
#define SNACKIS_PRIM_TYPE_HPP

#include <cstdint>
#include "snackis/core/type.hpp"

namespace snackis {
  template <typename ValT, typename BinT=ValT>
  struct PrimType: Type<ValT> {
    opt<ValT> null_val;
    
    PrimType(const str &name, opt<ValT> null_val=nullopt);
    bool is_null(const ValT &val) const override;
    ValT from_val(const Val &in) const override;
    Val to_val(const ValT &in) const override;
    ValT read(std::istream &in) const override;
    void write(const ValT &val, std::ostream &out) const override;
  };

  template <typename ValT, typename BinT>
  PrimType<ValT, BinT>::PrimType(const str &name, opt<ValT> null_val):
    Type<ValT>(name), null_val(null_val)
  { }

  template <typename ValT, typename BinT>
  bool PrimType<ValT, BinT>::is_null(const ValT &val) const {
    return null_val && val == *null_val; 
  }

  template <typename ValT, typename BinT>
  ValT PrimType<ValT, BinT>::from_val(const Val &in) const { return get<ValT>(in); }

  template <typename ValT, typename BinT>
  Val PrimType<ValT, BinT>::to_val(const ValT &in) const { return in; }

  template <typename ValT, typename BinT>
  ValT PrimType<ValT, BinT>::read(std::istream &in) const {
    BinT bin;
    in.read((char *)&bin, sizeof bin);
    return bin;
  }
  
  template <typename ValT, typename BinT>
  void PrimType<ValT, BinT>::write(const ValT &val, std::ostream &out) const {
    BinT bin(val);
    out.write((char *)&bin, sizeof bin);
  }

  using BoolType = PrimType<bool, int8_t>;
  extern BoolType bool_type;

  using Int64Type = PrimType<int64_t>;
  extern Int64Type int64_type;
}

#endif
