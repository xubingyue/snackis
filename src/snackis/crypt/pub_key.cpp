#include "snackis/crypt/key.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
namespace crypt {
  PubKey::PubKey(Key &key) {
    crypto_box_keypair(data, key.data);
  }
}}
