#ifndef SNACKIS_CRYPT_PUB_KEY_TYPE_HPP
#define SNACKIS_CRYPT_PUB_KEY_TYPE_HPP

#include <iostream>
#include "snackis/core/type.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
namespace crypt {
  struct PubKeyType: Type<PubKey> {
    PubKeyType();
    PubKey from_val(const Val &in) const override;
    Val to_val(const PubKey &in) const override;
    PubKey read(std::istream &in) const override;
    void write(const PubKey &val, std::ostream &out) const override;
  };

  extern PubKeyType pub_key_type;
}}

#endif
