#ifndef SNACKIS_FMT_HPP
#define SNACKIS_FMT_HPP

#include <vector>

#include "snackis/core/str.hpp"
#include "snackis/core/stream.hpp"
#include "snackis/core/tuple.hpp"

namespace snackis {  
  template <typename T>
  str fmt_arg(const T &arg) { return std::to_string(arg); }

  template <>
  str fmt_arg(const char *const &arg);
  
  template <>
  str fmt_arg(const str &arg);

  template <typename T>
  str fmt_arg(const std::vector<T> &arg) {
    OutStream buf;
    buf << '[';

    for (size_t i(0); i < arg.size(); i++) {
      if (i > 0) { buf << ' '; }
      buf << fmt_arg(arg[i]);
    };
    
    buf << ']';
    return buf.str();
  }

  template <typename...Args>
  str fmt(const str &in, const Args &..._args) {
    auto args(std::make_tuple(_args...));
    str out(in);
    size_t i = 0;

    for_each(args, [&i, &in, &out](auto &a) {
	const str val(fmt_arg(a));
	const str id("%" + to_str(i));
	size_t start = 0, end;

	while ((end = out.find(id, start)) != str::npos) {
	  if (end > 0 && out[end-1] == '%') {
	    out.erase(end-1, 1);
	    start = end-1+id.size();
	  } else {
	    out.replace(end, id.size(), val);
	    start = end+val.size();
	  }
	}

	i++;
      });

    return out;
  }
}

#endif
