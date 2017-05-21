#include "snackis/core/string_type.hpp"

namespace snackis {
  const StringType string_type;

  StringType::StringType(): Type<std::string>("string") {
  }
}
