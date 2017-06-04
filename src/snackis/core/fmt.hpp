#ifndef SNACKIS_FMT_HPP
#define SNACKIS_FMT_HPP

#include "snackis/core/buf.hpp"
#include "snackis/core/str.hpp"

namespace snackis {
  template <typename... Args, typename Func, std::size_t... Idx>
  void for_each(const std::tuple<Args...>& t, Func&& f, std::index_sequence<Idx...>) {
    (f(std::get<Idx>(t)), ...);
  }

  template <typename... Args, typename Func>
  void for_each(const std::tuple<Args...>& t, Func&& f) {
    for_each(t, f, std::index_sequence_for<Args...>{});
  }
  
  template <typename...Args>
  str fmt(const str &in, Args..._args) {
    auto args(std::make_tuple(std::forward<Args>(_args)...));
    str out(in);
    Buf buf;
    size_t i = 0;
    
    for_each(args, [&buf, &i, &in, &out](auto &a) {
	buf.str("");
	buf << a;
	const str val(buf.str());
	size_t start = 0, end = -1;
	const str id("%" + to_str(i));
	
	while ((end = out.find(id, start)) != str::npos) {
	  out.replace(end, id.size(), val);
	  start = end+val.size();
	}

	i++;
      });
    
    return out;
  }
}

#endif
