#ifndef SNACKIS_PRIM_TYPE_HPP
#define SNACKIS_PRIM_TYPE_HPP

#include <cstdint>
#include "snackis/core/type.hpp"

namespace snackis {
  template <typename ValT, typename BinT=ValT>
  struct PrimType: Type<ValT> {
    PrimType(const str &name);
    ValT from_val(const Val &in) const override;
    Val to_val(const ValT &in) const override;
    ValT read(std::istream &in) const override;
    void write(const ValT &val, std::ostream &out) const override;
  };

  template <typename ValT, typename BinT>
  PrimType<ValT, BinT>::PrimType(const str &name): Type<ValT>(name)
  { }

  template <typename ValT, typename BinT>
  ValT PrimType<ValT, BinT>::from_val(const Val &in) const { return get<ValT>(in); }

  template <typename ValT, typename BinT>
  Val PrimType<ValT, BinT>::to_val(const ValT &in) const { return in; }

  template <typename ValT, typename BinT>
  ValT PrimType<ValT, BinT>::read(std::istream &in) const {
    BinT val = -1;
    in.read((char *)&val, sizeof val);
    return val;
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
