#include "ctx.hpp"

namespace snackis {
namespace db {
  Ctx::Ctx(const Path &path): path(path), root(*this, nullptr), trans(&root) {
    create(path);
  }

  Ctx::~Ctx() { }

  std::string get_path(const Ctx &ctx, const std::string &fname) {
    return (ctx.path / fname).string(); 
  }

  void commit(Ctx &ctx) {
    commit(*ctx.trans);
  }
}}
