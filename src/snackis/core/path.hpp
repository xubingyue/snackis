#ifndef SNACKIS_PATH_HPP
#define SNACKIS_PATH_HPP

#define BOOST_NO_EXCEPTIONS

#include <boost/filesystem.hpp>

namespace snackis {
  using namespace boost;
  
  using Path = filesystem::path;
}

#endif
