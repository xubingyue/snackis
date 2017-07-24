#ifndef SNACKIS_CHAN_HPP
#define SNACKIS_CHAN_HPP

#include <atomic>
#include <mutex>
#include <vector>
#include "snackis/core/error.hpp"

namespace snackis {  
  template <typename T>
  struct Chan {
    const size_t max;
    std::vector<T> buf;
    size_t pos;
    std::atomic<size_t> size;
    std::mutex mutex;
    std::atomic<bool> closed;

    Chan(size_t max);
  };

  using ChanLock = std::unique_lock<std::mutex>;
  
  template <typename T>
  Chan<T>::Chan(size_t max):
    max(max), pos(0), size(0), closed(false)
  {
    buf.reserve(max);
  }

  template <typename T>
  void close(Chan<T> &c) {    
    ChanLock lock(c.mutex);
    CHECK(c.closed.load(), !_);
    c.closed.store(true);
  }

  template <typename T>
  bool put(Chan<T> &c, const T &it, bool wait=true) {
    while (true) {
      if (c.size.load() == c.max) {
	if (!wait || c.closed.load()) { break; }
	std::this_thread::yield();
	continue;
      }
    
      ChanLock lock(c.mutex);
      if (c.buf.size() == c.max) { continue; }
      
      c.buf.push_back(it);
      c.size++;
      return true;
    }

    return false;
  }

  template <typename T>
  opt<T> get(Chan<T> &c, bool wait=true) {
    while (true) {
      if (c.size.load() == 0) {
	if (!wait || c.closed.load()) { break; }
	std::this_thread::yield();
	continue;
      }

      ChanLock lock(c.mutex);

      if (c.pos == c.buf.size()) {
	continue;
      }
      
      auto out(c.buf[c.pos]);
      c.pos++;
      
      if (c.pos == c.buf.size()) {
	c.buf.clear();
	c.pos = 0;
      }
      
      c.size--;
      return out;
    }

    return nullopt;
  }
}

#endif
