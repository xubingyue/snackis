#include "snackis/crypt/pub_key_type.hpp"

namespace snackis {
namespace crypt {
  const PubKeyType pub_key_type;

  PubKeyType::PubKeyType(): Type<PubKey>("crypt::PubKey") {
  }

  PubKey PubKeyType::read(std::istream &in) const {
    return PubKey(in);
  }
  
  void PubKeyType::write(const PubKey &val, std::ostream &out) const {
    out.write((const char *)val.data, sizeof val.data);
  }
}}
