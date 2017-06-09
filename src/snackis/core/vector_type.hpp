#ifndef SNACKIS_VECTOR_TYPE_HPP
#define SNACKIS_VECTOR_TYPE_HPP

#include <vector>
#include "snackis/core/stream.hpp"
#include "snackis/core/type.hpp"

namespace snackis {
  template <typename ValT>
  struct VectorType: public Type<std::vector<ValT>> {
    Type<ValT> &val_type;
    
    VectorType(Type<ValT> &val_type);
    std::vector<ValT> from_val(const Val &in) const override;
    Val to_val(const std::vector<ValT> &in) const override;
    std::vector<ValT> read(std::istream &in) const override;
    void write(const std::vector<ValT> &val, std::ostream &out) const override;
  };

  template <typename ValT>
  VectorType<ValT> vector(Type<ValT> &val_type) {
    return VectorType<ValT>(val_type);
  }
  
  template <typename ValT>
  VectorType<ValT>::VectorType(Type<ValT> &val_type):
    Type<std::vector<ValT>>(fmt("Vector(%0)", val_type.name)), val_type(val_type)
  { }

  template <typename ValT>
  std::vector<ValT> VectorType<ValT>::from_val(const Val &in) const {
    Stream out(get<str>(in));
    return read(out);
  }

  template <typename ValT>
  Val VectorType<ValT>::to_val(const std::vector<ValT> &in) const {
    Stream out;
    write(in, out);
    return out.str();
  }

  template <typename ValT>
  std::vector<ValT> VectorType<ValT>::read(std::istream &in) const {
    int64_t len = -1;
    in.read((char *)&len, sizeof len);
    std::vector<ValT> out(len);
    for (size_t i = 0; i < len; i++) { out[i] = val_type.read(in); }
    return out;
  }
  
  template <typename ValT>
  void VectorType<ValT>::write(const std::vector<ValT> &in,
			       std::ostream &out) const {
    int64_t len = in.size();
    out.write(reinterpret_cast<const char *>(&len), sizeof len);
    for (const auto &v: in) { val_type.write(v, out); }
  }
}

#endif
