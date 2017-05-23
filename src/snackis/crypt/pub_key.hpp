#ifndef SNACKIS_CRYPT_PUB_KEY_HPP
#define SNACKIS_CRYPT_PUB_KEY_HPP

#include <sodium.h>

namespace snackis {
namespace crypt {
  struct Key;

  struct PubKey {
    unsigned char data[crypto_box_PUBLICKEYBYTES];

    PubKey(Key &key);
  };
}}

#endif
