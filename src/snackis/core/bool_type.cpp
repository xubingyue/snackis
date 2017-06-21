#include "bool_type.hpp"

namespace snackis {
  BoolType bool_type;

  BoolType::BoolType(): Type<bool>("Bool")
  { }

  bool BoolType::from_val(const Val &in) const { return get<bool>(in); }

  Val BoolType::to_val(const bool &in) const { return in; }

  bool BoolType::read(std::istream &in) const {
    int8_t val = -1;
    in.read((char *)&val, sizeof val);
    return val;
  }
  
  void BoolType::write(const bool &in, std::ostream &out) const {
    int8_t val(in);
    out.write((char *)&val, sizeof val);
  }
}
