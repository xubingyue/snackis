#include "snackis/core/string_type.hpp"

namespace snackis {
  const StringType string_type;

  StringType::StringType(): Type<std::string>("String") {
  }

  std::string StringType::read(std::istream &in) const {
    int64_t size;
    in >> size;
    char *data = new char[size+1];
    data[size] = 0;
    in.read(data, size);
    return std::string(data);
  }
  
  void StringType::write(const std::string &val, std::ostream &out) const {
    out << int64_t(val.size());
    out.write(val.c_str(), val.size());
  }
}
