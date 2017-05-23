#ifndef SNACKIS_CRYPT_PUB_KEY_TYPE_HPP
#define SNACKIS_CRYPT_PUB_KEY_TYPE_HPP

#include "snackis/core/type.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
namespace crypt {
  struct PubKeyType: public Type<PubKey> {
    PubKeyType();
  };

  extern const PubKeyType pub_key_type;
}}

#endif
