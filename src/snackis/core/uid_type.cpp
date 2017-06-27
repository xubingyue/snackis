#include "snackis/core/uid_type.hpp"

namespace snackis {
  UIdType uid_type;
  SetType<UId> uid_set_type(uid_type);

  UIdType::UIdType(): Type<UId>("UId") { }

  bool UIdType::is_null(const UId &val) const { return val == null_uid; }

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
