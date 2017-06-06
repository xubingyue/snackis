#ifndef SNACKIS_PATH_HPP
#define SNACKIS_PATH_HPP

#include <experimental/filesystem>

namespace snackis {
  using Path = std::experimental::filesystem::path;
  using PathIter = std::experimental::filesystem::directory_iterator;
  
  bool create_path(const Path &p);
  bool path_exists(const Path &p);
}

#endif
