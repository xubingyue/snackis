#include "snackis/core/uid_type.hpp"

namespace snackis {
  const UIdType uid_type;

  UIdType::UIdType(): Type<UId>("UId") { }

  UId UIdType::from_val(const Val &in) const { return get<UId>(in); }

  Val UIdType::to_val(const UId &in) const { return in; }

  UId UIdType::read(std::istream &in) const {
    UId id(false);
    in.read(reinterpret_cast<char *>(id.val), sizeof(id.val));
    return id;
  }
  
  void UIdType::write(const UId &id, std::ostream &out) const {
    out.write(reinterpret_cast<const char *>(id.val), sizeof(id.val));
  }
}
