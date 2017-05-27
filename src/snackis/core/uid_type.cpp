#include "snackis/core/uid_type.hpp"

namespace snackis {
  const UIdType uid_type;

  UIdType::UIdType(): Type<UId>("UId") { }

  UId UIdType::from_val(const Val &in) const { return get<UId>(in); }

  Val UIdType::to_val(const UId &in) const { return in; }

  UId UIdType::read(std::istream &in) const {
    UId val;
    in.read(reinterpret_cast<char *>(&val), 16);
    return val;
  }
  
  void UIdType::write(const UId &val, std::ostream &out) const {
    out.write(reinterpret_cast<const char *>(&val), 16);
  }
}
