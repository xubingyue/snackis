#ifndef SNACKIS_CRYPT_KEY_HPP
#define SNACKIS_CRYPT_KEY_HPP

#include "snackis/core/data.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
namespace crypt {
  struct Key {
    unsigned char data[crypto_box_SECRETKEYBYTES];
    Key();
    Key(PubKey &pub);
    Key(std::istream &in);
  };  

  bool operator <(const Key &x, const Key &y);
    
  Data encrypt(const Key &key, const PubKey &pub_key,
	       const unsigned char *in,
	       size_t len);

  Data decrypt(const Key &key, const PubKey &pub_key,
	       const unsigned char *in,
	       size_t len);
}}



#endif
