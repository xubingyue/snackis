#ifndef SNACKIS_CRYPT_KEY_TYPE_HPP
#define SNACKIS_CRYPT_KEY_TYPE_HPP

#include <iostream>
#include "snackis/core/type.hpp"
#include "snackis/crypt/key.hpp"

namespace snackis {
namespace crypt {
  struct KeyType: public Type<Key> {
    KeyType();
    Key from_val(const Val &in) const override;
    Val to_val(const Key &in) const override;
    Key read(std::istream &in) const override;
    void write(const Key &val, std::ostream &out) const override;
  };

  extern KeyType key_type;
}}

#endif
