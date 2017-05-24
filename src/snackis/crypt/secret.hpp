#ifndef SNACKIS_CRYPT_SECRET_HPP
#define SNACKIS_CRYPT_SECRET_HPP

#include <sodium.h>
#include <string>
#include <vector>

namespace snackis {
namespace crypt {
  struct Secret {
    static const size_t
    SALT_SIZE = crypto_pwhash_scryptsalsa208sha256_SALTBYTES,
      KEY_SIZE = crypto_aead_chacha20poly1305_IETF_KEYBYTES,
      SIZE = SALT_SIZE + KEY_SIZE,
      NONCE_SIZE = crypto_aead_chacha20poly1305_IETF_NPUBBYTES;

    unsigned char data[SIZE];

    Secret();
  };

  void init(Secret &sec, const std::string &key);
  const unsigned char *hash(const Secret &sec);

  std::vector<unsigned char> encrypt(const Secret &secret,
				     const unsigned char *in,
				     size_t len);
  
  std::vector<unsigned char> decrypt(const Secret &secret,
				     const unsigned char *in,
				     size_t len);
}}
  
#endif
