#include "snackis/core/data_type.hpp"

namespace snackis {
  const DataType data_type;

  DataType::DataType(): Type<Data>("Data") { }

  Data DataType::read(std::istream &in) const {
    int64_t size = -1;
    in.read((char *)&size, sizeof size);
    Data out(size);
    in.read((char *)&out[0], size);
    return out;
  }
  
  void DataType::write(const Data &val, std::ostream &out) const {
    int64_t size = val.size();
    out.write((const char *)&size, sizeof size);
    out.write((char *)&val[0], size);
  }
}
