#include "snackis/core/uid_type.hpp"

namespace snackis {
  const UIdType uid_type;

  UIdType::UIdType(): Type<UId>("uid") {
  }
}
