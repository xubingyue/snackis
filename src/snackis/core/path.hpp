#ifndef SNACKIS_PATH_HPP
#define SNACKIS_PATH_HPP

#include <boost/filesystem.hpp>

namespace snackis {
  namespace fs = boost::filesystem;

  using Path = fs::path;

  bool create_path(const Path &path);
  bool path_exists(const Path &path);
}

#endif
