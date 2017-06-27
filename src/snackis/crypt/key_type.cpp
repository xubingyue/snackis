#include "snackis/crypt/key_type.hpp"

namespace snackis {
namespace crypt {
  KeyType key_type;

  KeyType::KeyType(): Type<Key>("crypt::Key")
  { }

  bool KeyType::is_null(const Key &val) const { return val == null_key; }

  Key KeyType::from_val(const Val &in) const { return get<Key>(in); }

  Val KeyType::to_val(const Key &in) const { return in; }

  Key KeyType::read(std::istream &in) const {
    return Key(in);
  }
  
  void KeyType::write(const Key &val, std::ostream &out) const {
    out.write((const char *)val.data, sizeof val.data);
  }
}}
