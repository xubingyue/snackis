#ifndef SNACKIS_DATA_HPP
#define SNACKIS_DATA_HPP

#include <vector>
#include "snackis/core/path.hpp"

namespace snackis {
  using Data = std::vector<unsigned char>;

  void dump_data(const Data &buf, const Path &p);
  Data slurp_data(const Path &in);
}

#endif
