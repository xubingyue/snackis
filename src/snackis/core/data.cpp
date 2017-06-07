#include <fstream>
#include "snackis/core/defer.hpp"
#include "snackis/core/data.hpp"

namespace snackis {
  void dump_data(const Data &buf, const Path &p) {
    std::ofstream f;
    DEFER({ f.close(); });

    f.open(p, std::ios::out | std::ios::trunc | std::ios::binary);
    f.write(reinterpret_cast<const char *>(&buf[0]), buf.size());
  }
	   
  Data slurp_data(const Path &p) {
    std::ifstream f;
    f.open(p, std::ios::in | std::ios::binary);
    DEFER({ f.close(); });
    
    f.seekg(0, std::ios::end);
    Data buf(f.tellg(), 0);
    f.seekg(0);
    f.read(reinterpret_cast<char *>(&buf[0]), buf.size());
    return buf;
  }
}
