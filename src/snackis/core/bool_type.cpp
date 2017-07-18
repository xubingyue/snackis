#include "snackis/core/bool_type.hpp"

namespace snackis {
  BoolType bool_type;

  BoolType::BoolType(): Type<bool>("Bool")
  { }

  bool BoolType::from_val(const Val &in) const { return get<bool>(in); }

  Val BoolType::to_val(const bool &in) const { return in; }

  bool BoolType::read(std::istream &in) const {
    uint8_t val;
    in.read((char *)&val, sizeof val);
    return val;
  }
  
  void BoolType::write(const bool &val, std::ostream &out) const {
    const uint8_t out_val(val);
    out.write((char *)&out_val, sizeof(out_val));
  }
}
