#ifndef SNACKIS_DATA_TYPE_HPP
#define SNACKIS_DATA_TYPE_HPP

#include "snackis/core/data.hpp"
#include "snackis/core/type.hpp"

namespace snackis {
  struct DataType: public Type<Data> {
    DataType();
    Data read(std::istream &in) const override;
    void write(const Data &val, std::ostream &out) const override;
  };

  extern const DataType data_type;
}

#endif
