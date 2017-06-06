#include <sodium.h>

#include "snackis/core/error.hpp"
#include "snackis/core/str.hpp"

namespace snackis {
  const str whitespace(" \t\r\n");
  
  str trim(const str& in) {
    const size_t i = in.find_first_not_of(whitespace);
    if (i == str::npos) { return ""; }
    const size_t j = in.find_last_not_of(whitespace);
    return in.substr(i, (j-i+1));
  }

  int64_t to_int64(const str &in) {
    return std::stoll(in);
  }

  str bin_hex(const unsigned char *in, size_t len) {
    const size_t out_len = len*2+1;
    Data out(out_len, 0);
    
    if (!sodium_bin2hex(reinterpret_cast<char *>(&out[0]), out_len, in, len)) {
      ERROR(Core, "Hex-encoding failed");
    }

    return str(reinterpret_cast<const char *>(&out[0]));
  }
  
  Data hex_bin(const str &in) {
    Data out(in.size()/2, 0);
    size_t len;
    
    if (sodium_hex2bin(&out[0], out.size(),
		       in.c_str(), in.size(),
		       nullptr,
		       &len,
		       nullptr)) {
      ERROR(Core, "Hex-decoding failed");
    }

    out.resize(len);
    return out;
  }
}
