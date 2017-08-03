#include "snabel/coro.hpp"
#include "snabel/scope.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  Label::Label(int64_t dep, int64_t pc):
    depth(dep), pc(pc)
  { }
  
  Scope::Scope(const Scope &src):
    coro(src.coro), labels(src.labels), env(src.env)
  { }

  Scope::Scope(Coro &cor):
    coro(cor)
  { }

  Box *find_env(Scope &scp, const str &n) {
    auto fnd(scp.env.find(n));
    if (fnd == scp.env.end()) { return nullptr; }
    return &fnd->second;
  }

  Box get_env(Scope &scp, const str &n) {
    auto fnd(find_env(scp, n));
    CHECK(fnd, _);
    return *fnd;
  }

  void put_env(Scope &scp, const str &n, const Box &val) {
    auto fnd(scp.env.find(n));

    if (fnd != scp.env.end()) {
      ERROR(Snabel, fmt("Name '%0' is already bound to: %1", n, fnd->second));
    }

    scp.env.emplace(std::piecewise_construct,
		    std::forward_as_tuple(n),
		    std::forward_as_tuple(val));
  }

  bool rem_env(Scope &scp, const str &n) {
    return scp.env.erase(n) == 1;
  }
}
