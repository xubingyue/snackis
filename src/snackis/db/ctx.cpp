#include "ctx.hpp"

namespace snackis {
namespace db {
  Ctx::Ctx(const Path &path): path(path), secret(nullptr), root(*this, nullptr),
			      trans(&root) {
    create(path);
  }

  Ctx::~Ctx() { }

  str get_path(const Ctx &ctx, const str &fname) {
    return (ctx.path / fname).string(); 
  }

  void commit(Ctx &ctx) { commit(*ctx.trans); }
}}
