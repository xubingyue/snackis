#include "snackis/core/error.hpp"
#include "snackis/core/type.hpp"

namespace snackis {
  const NullType null_type;

  BasicType::BasicType(const str &name): name(name) { }

  NullType::NullType(): BasicType("Null") { }

  Val NullType::read_val(std::istream &in) const {
    ERROR(Core, "Reading null type");
  }
  
  void NullType::write_val(const Val &val, std::ostream &out) const {
    ERROR(Core, "Writing null type");
  }
}
