#ifndef SNACKIS_CRYPT_PUB_KEY_HPP
#define SNACKIS_CRYPT_PUB_KEY_HPP

#include <sodium.h>
#include <istream>

namespace snackis {
namespace crypt {
  struct Key;

  struct PubKey {
    unsigned char data[crypto_box_PUBLICKEYBYTES];
    PubKey();
    PubKey(std::istream &in);
  };

  extern const PubKey null_pub_key;

  bool operator ==(const PubKey &x, const PubKey &y);
  bool operator <(const PubKey &x, const PubKey &y);
}}

#endif
