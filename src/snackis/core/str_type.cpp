#include "snackis/core/str_type.hpp"

namespace snackis {
  StrType str_type;

  StrType::StrType(): Type<str>("Str") { }

  bool StrType::is_null(const str &val) const { return val.empty(); }

  str StrType::from_val(const Val &in) const { return get<str>(in); }

  Val StrType::to_val(const str &in) const { return in; }

  str StrType::read(std::istream &in) const {
    int64_t size = -1;
    in.read((char *)&size, sizeof size);
    std::vector<char> data(size);
    in.read(&data[0], size);
    return str(data.begin(), data.end());
  }
  
  void StrType::write(const str &val, std::ostream &out) const {
    int64_t size = val.size();
    out.write((const char *)&size, sizeof size);
    out.write(val.data(), size);
  }
}
