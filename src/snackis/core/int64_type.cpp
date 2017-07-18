#include "snackis/core/error.hpp"
#include "snackis/core/int64_type.hpp"

namespace snackis {
  Int64Type int64_type;

  Int64Type::Int64Type(): Type<int64_t>("Int64")
  { }

  int64_t Int64Type::from_val(const Val &in) const { return get<int64_t>(in); }

  Val Int64Type::to_val(const int64_t &in) const { return in; }

  int64_t Int64Type::read(std::istream &in) const {
    uint8_t len;
    in.read((char *)&len, sizeof len);
    std::vector<char> data(len);
    in.read(&data[0], len);
    const str str_val(data.begin(), data.end());
    return to_int64(str_val);    
  }
  
  void Int64Type::write(const int64_t &val, std::ostream &out) const {
    const str str_val(to_str(val));
    uint8_t len(str_val.size());
    out.write((char *)&len, sizeof len);
    out.write(str_val.data(), len);
  }
}
