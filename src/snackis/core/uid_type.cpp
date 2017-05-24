#include "snackis/core/uid_type.hpp"

namespace snackis {
  const UIdType uid_type;

  UIdType::UIdType(): Type<UId>("UId") {
  }

  UId UIdType::read(std::istream &in) const {
    UId val;
    in >> val;
    return val;
  }
  
  void UIdType::write(const UId &val, std::ostream &out) const {
    out << val;
  }
}
