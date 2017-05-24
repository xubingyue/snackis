#ifndef SNACKIS_UID_TYPE_HPP
#define SNACKIS_UID_TYPE_HPP

#include "snackis/core/type.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct UIdType: public Type<UId> {
    UIdType();
    UId read(std::istream &in) const override;
    void write(const UId &val, std::ostream &out) const override;
  };

  extern const UIdType uid_type;
}

#endif
