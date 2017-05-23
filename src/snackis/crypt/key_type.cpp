#include "snackis/crypt/key_type.hpp"

namespace snackis {
namespace crypt {
  const KeyType key_type;

  KeyType::KeyType(): Type<Key>("crypt::Key") {
  }
}}
