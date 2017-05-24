#include "snackis/core/string_type.hpp"

namespace snackis {
  const StringType string_type;

  StringType::StringType(): Type<std::string>("String") {
  }

  std::string StringType::read(std::istream &in) const {
    int64_t size;
    in.read((char *)&size, sizeof size);
    char *data = new char[size+1];
    data[size] = 0;
    in.read(data, size);
    return std::string(data);
  }
  
  void StringType::write(const std::string &val, std::ostream &out) const {
    int64_t size = val.size();
    out.write((const char *)&size, sizeof size);
    out.write(val.c_str(), size);
  }
}
