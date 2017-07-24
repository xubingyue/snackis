#ifndef SNACKIS_CHAN_HPP
#define SNACKIS_CHAN_HPP

#include <atomic>
#include <condition_variable>
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
    std::condition_variable get_ok, put_ok;
    bool closed;

    Chan(size_t max);
  };

  using ChanLock = std::unique_lock<std::mutex>;
  
  const int CHAN_RETRIES(10);

  template <typename T>
  Chan<T>::Chan(size_t max):
    max(max), pos(0), size(0), closed(false)
  { }

  template <typename T>
  void close(Chan<T> &c) {    
    ChanLock lock(c.mutex);
    CHECK(c.closed, !_);
    c.closed = true;
    c.get_ok.notify_all();
    c.put_ok.notify_all();
  }

  template <typename T>
  bool put(Chan<T> &c, const T &it, bool wait=true) {
    if (c.size.load() == c.max) {
      if (!wait) { return false; }
      int i(0);

      do { 
	std::this_thread::yield();
	i++;
      } while (c.size.load() == c.max && i <= CHAN_RETRIES);
    }
    
    ChanLock lock(c.mutex);
    if (c.closed) { return false; }

    if (wait && c.buf.size() == c.max) {
      c.put_ok.wait(lock, [&c](){ return c.closed || c.buf.size() < c.max; });
    }

    if (c.buf.size() == c.max) { return false; }
    c.buf.push_back(it);
    c.size++;
    c.get_ok.notify_one();
    return true;
  }

  template <typename T>
  opt<T> get(Chan<T> &c, bool wait=true) {
    if (c.size.load() == 0) {
      if (!wait) { return nullopt; }
      int i(0);

      do { 
	std::this_thread::yield(); 
	i++;
      } while (c.size.load() == 0 && i <= CHAN_RETRIES);
    }

    ChanLock lock(c.mutex);
    
    if (wait && c.pos == c.buf.size()) {
      c.get_ok.wait(lock, [&c](){ return c.closed || c.pos < c.buf.size(); });
    }
    
    if (c.pos == c.buf.size()) { return nullopt; }
    auto out(c.buf[c.pos]);
    c.pos++;

    if (c.pos == c.buf.size()) {
      c.buf.clear();
      c.pos = 0;
    }
    
    c.size--;
    c.put_ok.notify_one();
    return out;
  }
}

#endif
