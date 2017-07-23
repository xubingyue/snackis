#include <fstream>
#include "snackis/ctx.hpp"
#include "snackis/snackis.hpp"
#include "snackis/crypt/error.hpp"
#include "snackis/db/proc.hpp"

namespace snackis {
namespace db {
  Ctx::Ctx(Proc &p, size_t max_buf):
    proc(p), inbox(max_buf), trans(nullptr)
  { 
    Msg msg(MSG_CONNECT);
    set(msg, Msg::SENDER, this);
    put(proc.inbox, msg);
    
    auto res(get(inbox));
    CHECK(res, _);
    CHECK(res->type == MSG_OK, _);
  }

  Ctx::~Ctx() { 
    Msg msg(MSG_DISCONNECT);
    set(msg, Msg::SENDER, this);
    put(proc.inbox, msg);
    get(inbox);
  }

  Path get_path(const Ctx &ctx, const str &fname) {
    return ctx.proc.path / fname; 
  }

  Trans &get_trans(Ctx &ctx) {
    CHECK(ctx.trans, _);
    return *ctx.trans;
  }

  bool pass_exists(const Ctx &ctx) {
    return path_exists(get_path(ctx, "pass"));
  }

  void init_pass(Ctx &ctx, const str &pass) {
    ctx.secret = crypt::Secret();
    init_salt(*ctx.secret);
    init(*ctx.secret, pass);
    Data edata(crypt::encrypt(*ctx.secret,
			      reinterpret_cast<const unsigned char*>(pass.c_str()),
			      pass.size()));
    std::ofstream file;
    file.open(get_path(ctx, "pass").string(), std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char *>(ctx.secret->data),
	       crypt::Secret::SALT_SIZE);
    file.write(reinterpret_cast<const char*>(&edata[0]), edata.size());
    file.close();
  }

  bool login(Ctx &ctx, const str &pass) {
    TRY(try_login);
    crypt::Secret secret;
    
    std::ifstream file;
    file.open(get_path(ctx, "pass").string(), std::ios::in | std::ios::binary);
    file.seekg(0, std::ios::end);
    const std::streamsize size =
      std::streamsize(file.tellg())-std::streamsize(crypt::Secret::SALT_SIZE);
    file.seekg(0);
    file.read(reinterpret_cast<char *>(secret.data), crypt::Secret::SALT_SIZE);
    init(secret, pass);
    Data edata(size);
    file.read(reinterpret_cast<char *>(&edata[0]), size);
    file.close();

    if (!try_login.errors.empty()) { return false; }

    Data ddata;
    ddata = crypt::decrypt(secret, &edata[0], edata.size());
    if (!try_login.errors.empty()) { return false; }
    
    if (str(ddata.begin(), ddata.end()) != pass) { return false; }
    ctx.secret = secret;
    return true;
  }

  void slurp(Ctx &ctx) {
    TRY(try_slurp);
    for (auto t: ctx.tables) {
      if (path_exists(t.second->path)) { t.second->slurp(); }
    }
  }

  int64_t rewrite(Ctx &ctx) {
    TRY(try_rewrite);
    Msg msg(MSG_REWRITE);
    set(msg, Msg::SENDER, &ctx);
    put(ctx.proc.inbox, msg);
    auto res(get(ctx.inbox));
    return (res && res->type == MSG_OK) ? get(*res, Msg::RECLAIMED) : -1;
  }

  int64_t refresh(Ctx &ctx) {
    TRY(try_refresh);
    Msg msg(MSG_REFRESH);
    set(msg, Msg::SENDER, &ctx);
    put(ctx.proc.inbox, msg);
    auto res(get(ctx.inbox));
    
    if (res && res->type == MSG_OK) {
      auto cs(get(*res, Msg::CHANGES));
      for (auto &c: cs) { c->apply(ctx); }
      return cs.size();
    }

    return -1;
  }
}}
