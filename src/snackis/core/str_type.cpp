#include "snackis/core/str_type.hpp"

namespace snackis {
  const StrType str_type;

  StrType::StrType(): Type<str>("Str") { }

  str StrType::from_val(const Val &in) const { return get<str>(in); }

  Val StrType::to_val(const str &in) const { return in; }

  str StrType::read(std::istream &in) const {
    int64_t size = -1;
    in.read((char *)&size, sizeof size);
    char *data = new char[size+1];
    data[size] = 0;
    in.read(data, size);
    return str(data);
  }
  
  void StrType::write(const str &val, std::ostream &out) const {
    int64_t size = val.size();
    out.write((const char *)&size, sizeof size);
    out.write(val.c_str(), size);
  }
}
