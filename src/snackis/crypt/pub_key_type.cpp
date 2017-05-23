#include "snackis/crypt/pub_key_type.hpp"

namespace snackis {
namespace crypt {
  const PubKeyType pub_key_type;

  PubKeyType::PubKeyType(): Type<PubKey>("crypt::PubKey") {
  }
}}
