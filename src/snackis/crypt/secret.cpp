#include <cstring>
#include "snackis/crypt/error.hpp"
#include "snackis/crypt/secret.hpp"

namespace snackis {
namespace crypt {
  Secret::Secret() {
    memset(data, 0, SIZE);
  }

  void init(Secret &sec, const std::string &key) {
    randombytes_buf(sec.data, Secret::SALT_SIZE);

    if (crypto_pwhash_scryptsalsa208sha256(sec.data + Secret::SALT_SIZE,
					   Secret::KEY_SIZE,
					   key.c_str(), key.size(),
					   sec.data,
					   crypto_pwhash_scryptsalsa208sha256_OPSLIMIT_INTERACTIVE,
					   crypto_pwhash_scryptsalsa208sha256_MEMLIMIT_INTERACTIVE) != 0) {
      ERROR(Crypt, "Failed hashing key");
    }
  }

  const unsigned char *hash(const Secret &sec) {
    return sec.data + Secret::SALT_SIZE;
  }
  
  std::vector<unsigned char> encrypt(const Secret &sec,
				     const unsigned char *in,
				     size_t len) {
    std::vector<unsigned char> out;
    const size_t DATA_SIZE = crypto_aead_chacha20poly1305_IETF_ABYTES+len;
    out.resize(Secret::NONCE_SIZE+DATA_SIZE, 0);
    randombytes_buf(&out[0], Secret::NONCE_SIZE);

    unsigned long long clen;
    crypto_aead_chacha20poly1305_ietf_encrypt(&out[Secret::NONCE_SIZE], &clen,
					      in, len,
					      NULL, 0,
					      NULL, &out[0], hash(sec));
    out.resize(Secret::NONCE_SIZE+clen);
    return out;
  }

  std::vector<unsigned char> decrypt(const Secret &secret,
				     const unsigned char *in,
				     size_t len) {
    std::vector<unsigned char> out;
    out.resize(len-Secret::NONCE_SIZE-crypto_aead_chacha20poly1305_IETF_ABYTES);

    unsigned long long dlen;
    if (crypto_aead_chacha20poly1305_ietf_decrypt(&out[0],
						  &dlen,
						  NULL,
						  in+Secret::NONCE_SIZE,
						  len-Secret::NONCE_SIZE,
						  NULL, 0,
						  in, hash(secret)) != 0) {
      ERROR(Crypt, "Failed decrypting secret message");
    }

    out.resize(dlen);
    return out;
  }
}}
