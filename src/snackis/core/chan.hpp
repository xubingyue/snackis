#ifndef SNACKIS_CHAN_HPP
#define SNACKIS_CHAN_HPP

#include <shared_mutex>
#include "snackis/core/error.hpp"

namespace snackis {  
  template <typename T>
  struct Chan {
    const size_t max;
    std::deque<T> buf;
    std::mutex mutex;
    std::condition_variable get_ok, put_ok;
    bool closed;

    Chan(size_t max);
  };

  using ChanRLock = std::shared_lock<std::mutex>;
  using ChanWLock = std::unique_lock<std::mutex>;
  
  template <typename T>
  Chan<T>::Chan(size_t max):
    max(max), closed(false)
  { }

  template <typename T>
  void close(Chan<T> &c) {
    ChanWLock lock(c.mutex);
    CHECK(!c.closed, _);
    c.closed = true;
    c.get_ok.notify_all();
    c.put_ok.notify_all();
  }

  template <typename T>
  bool closed(Chan<T> &c) {
    ChanRLock lock(c.mutex);
    return c.closed;
  }

  template <typename T>
  bool put(Chan<T> &c, const T &it, bool wait=true) {
    ChanWLock lock(c.mutex);

    if (wait && c.buf.size() == c.max) {
      c.put_ok.wait(lock, [&c](){ return c.closed || c.buf.size() < c.max; });
    }

    if (c.buf.size() == c.max) { return false; }
    c.buf.push_back(it);
    c.get_ok.notify_one();
    return true;
  }

  template <typename T>
  opt<T> get(Chan<T> &c, bool wait=true) {
    ChanWLock lock(c.mutex);
    
    if (wait && c.buf.empty()) {
      c.get_ok.wait(lock, [&c](){ return c.closed || !c.buf.empty(); });
    }
    
    if (c.buf.empty()) { return nullopt; }
    auto out(c.buf.front());
    c.buf.pop_front();
    c.put_ok.notify_one();
    return out;
  }

  template <typename T>
  void drain(Chan<T> &c) {
    ChanWLock lock(c.mutex);

    if (!c.buf.empty()) {
      c.put_ok.wait(lock, [&c](){ return c.buf.empty(); });
    }
  }
}

#endif
