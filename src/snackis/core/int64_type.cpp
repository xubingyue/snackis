#include "int64_type.hpp"

namespace snackis {
  const Int64Type int64_type;

  Int64Type::Int64Type(): Type<int64_t>("Int64") {
  }

  int64_t Int64Type::read(std::istream &in) const {
    int64_t val = -1;
    in.read((char *)&val, sizeof val);
    return val;
  }
  
  void Int64Type::write(const int64_t &val, std::ostream &out) const {
    out.write((char *)&val, sizeof val);
  }
}
