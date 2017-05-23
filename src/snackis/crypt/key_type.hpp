#ifndef SNACKIS_CRYPT_KEY_TYPE_HPP
#define SNACKIS_CRYPT_KEY_TYPE_HPP

#include "snackis/core/type.hpp"
#include "snackis/crypt/key.hpp"

namespace snackis {
namespace crypt {
  struct KeyType: public Type<Key> {
    KeyType();
  };

  extern const KeyType key_type;
}}

#endif
