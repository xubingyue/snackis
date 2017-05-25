#ifndef SNACKIS_MUTEX_HPP
#define SNACKIS_MUTEX_HPP

#include <boost/thread/shared_mutex.hpp>

namespace snackis {
  using Mutex = boost::shared_mutex;
  using RLock = boost::shared_lock<Mutex>;
  using WLock = boost::unique_lock<Mutex>;
}

#endif
