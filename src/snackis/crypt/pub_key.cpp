#include <cstring>

#include "snackis/crypt/key.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
namespace crypt {
  PubKey::PubKey() {
    memset(data, 0, sizeof data);
  }

  PubKey::PubKey(std::istream &in) {
    in.read((char *)data, sizeof data);
  }

  bool operator <(const PubKey &x, const PubKey &y) {
    return memcmp(x.data, y.data, crypto_box_PUBLICKEYBYTES) < 0;
  }
}}
