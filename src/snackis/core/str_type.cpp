#include "snackis/core/int64_type.hpp"
#include "snackis/core/str_type.hpp"

namespace snackis {
  StrType str_type;

  StrType::StrType(): Type<str>("Str") { }

  bool StrType::is_null(const str &val) const { return val.empty(); }

  str StrType::from_val(const Val &in) const { return get<str>(in); }

  Val StrType::to_val(const str &in) const { return in; }

  str StrType::read(std::istream &in) const {
    int64_t len(int64_type.read(in)); 
    std::vector<char> data(len);
    in.read(&data[0], len);
    return str(data.begin(), data.end());
  }
  
  void StrType::write(const str &val, std::ostream &out) const {
    const int64_t len(val.size());
    int64_type.write(len, out);
    out.write(val.data(), len);
  }
}
