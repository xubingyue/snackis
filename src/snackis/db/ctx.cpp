#include <fstream>
#include "snackis/crypt/error.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/ctx.hpp"

namespace snackis {
namespace db {
  Ctx::Ctx(const Path &path): path(path) {
    create_path(path);
  }

  Ctx::~Ctx() { }

  Path get_path(const Ctx &ctx, const str &fname) {
    return ctx.path / fname; 
  }

  void log(const Ctx &ctx, const str &msg) {
    if (ctx.log) {
      WLock lock(const_cast<Ctx &>(ctx).log_mutex);
      ctx.log.get()(msg);
    }
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
    crypt::Secret secret;
    
    std::ifstream file;
    file.open(get_path(ctx, "pass").string(), std::ios::in | std::ios::binary);
    file.seekg(0, std::ios::end);
    const std::streamsize size =
      file.tellg()-std::streamsize(crypt::Secret::SALT_SIZE);
    file.seekg(0);
    file.read(reinterpret_cast<char *>(secret.data), crypt::Secret::SALT_SIZE);
    init(secret, pass);
    Data edata(size);
    file.read(reinterpret_cast<char *>(&edata[0]), size);
    file.close();

    Data ddata;
    try {
      ddata = crypt::decrypt(secret, &edata[0], edata.size());
    } catch (const crypt::CryptError &e) {
      return false;
    }
    
    if (str(ddata.begin(), ddata.end()) != pass) { return false; }
    ctx.secret = secret;
    return true;
  }

  void open(Ctx &ctx) {
    for (auto t: ctx.tables) { open(*t); }
  }

  void dirty_file(Ctx &ctx, std::ostream &file) {
    ctx.dirty_files.insert(&file);
  }

  void flush(Ctx &ctx) {
    for (auto f: ctx.dirty_files) { f->flush(); }
    ctx.dirty_files.clear();
  }

  void slurp(Ctx &ctx) {
    for (auto t: ctx.tables) { t->slurp(); }
  }
}}
