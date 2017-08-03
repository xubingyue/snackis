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
    coro(src.coro), labels(src.labels), envs(src.envs)
  { }

  Scope::Scope(Coro &cor):
    coro(cor)
  {
    push_env(*this);
  }

  Env &curr_env(Scope &scp) {
    CHECK(!scp.envs.empty(), _);
    return scp.envs.back();
  }

  Env &push_env(Scope &scp) {
    if (scp.envs.empty()) {
      return scp.envs.emplace_back(Env());
    }

    return scp.envs.emplace_back(Env(curr_env(scp)));
  }
  
  void pop_env(Scope &scp) {
    CHECK(!scp.envs.empty(), _);
    scp.envs.pop_back();
    CHECK(!scp.envs.empty(), _);
  }

  Box *find_env(Scope &scp, const str &n) {
    auto &env(curr_env(scp));
    auto fnd(env.find(n));
    if (fnd == env.end()) { return nullptr; }
    return &fnd->second;
  }

  Box get_env(Scope &scp, const str &n) {
    auto fnd(find_env(scp, n));
    CHECK(fnd, _);
    return *fnd;
  }

  void put_env(Scope &scp, const str &n, const Box &val) {
    auto &env(curr_env(scp));
    auto fnd(env.find(n));

    if (fnd != env.end()) {
      ERROR(Snabel, fmt("Name '%0' is already bound to: %1", n, fnd->second));
    }

    env.emplace(std::piecewise_construct,
		std::forward_as_tuple(n),
		std::forward_as_tuple(val));
  }

  bool rem_env(Scope &scp, const str &n) {
    return curr_env(scp).erase(n) == 1;
  }
}
