#include <cassert>
#include <iostream>

#include "snackis/ctx.hpp"
#include "snackis/snackis.hpp"
#include "snackis/core/data.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/set_type.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/stream.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/crypt/key.hpp"
#include "snackis/crypt/secret.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/table.hpp"
#include "snackis/net/imap.hpp"

using namespace snackis;
using namespace snackis::db;

static void str_tests() {
  assert(find_ci("foo", "bar") == str::npos);
  assert(find_ci("foobar", "BAR") == 3);
}

static void crypt_secret_tests() {
  using namespace snackis::crypt;
  str key("secret key");
  Secret sec;
  init_salt(sec);
  init(sec, key);
  str msg("secret message");
  
  Data cmsg(encrypt(sec, (const unsigned char *)msg.c_str(), msg.size())),
    dmsg(decrypt(sec, &cmsg[0], cmsg.size()));

  assert(str(dmsg.begin(), dmsg.end()) == msg);
}

static void crypt_key_tests() {
  using namespace snackis::crypt;
  PubKey foo_pub, bar_pub;
  Key foo(foo_pub), bar(bar_pub);
  str msg("secret message");

  Data cmsg(encrypt(foo, bar_pub, (const unsigned char *)msg.c_str(), msg.size())),
    dmsg(decrypt(bar, foo_pub, &cmsg[0], cmsg.size()));

  assert(str(dmsg.begin(), dmsg.end()) == msg);
}

struct Foo {
  int64_t fint64;
  str fstr;
  Time ftime;
  UId fuid;
  std::set<int64_t> fset;
  Foo(): fint64(0), ftime(now()) { }

  Foo(db::Table<Foo> &tbl, db::Rec<Foo> &rec): fuid(false) {
    copy(tbl, *this, rec);
  }
};

namespace snackis {
  template <>
  str fmt_arg(const Foo &arg) {
    return "Foo";
  }
}

static void fmt_tests() {
  assert(fmt("%0 %1\n", "abc", 42) == "abc 42\n");
  assert(fmt("%0 %%1 %1", "abc", 42) == "abc %1 42");
  assert(fmt("%0 %1 %2", "abc", Foo(), "42") == "abc Foo 42");
}

static void schema_tests() {
  const Col<Foo, int64_t> col("int64", int64_type, &Foo::fint64); 
  Schema<Foo> scm({&col});

  Rec<Foo> foo, bar;
  set(foo, col, int64_t(42));
  assert(compare(scm, foo, bar) == 1);

  set(bar, col, int64_t(42));
  assert(compare(scm, foo, bar) == 0);
  
  set(bar, col, int64_t(43));
  assert(compare(scm, foo, bar) == -1);
}

const Col<Foo, int64_t> int64_col("int64", int64_type, &Foo::fint64); 
const Col<Foo, str> str_col("str", str_type, &Foo::fstr); 
const Col<Foo, Time> time_col("time", time_type, &Foo::ftime); 
const Col<Foo, UId> uid_col("uid", uid_type, &Foo::fuid); 
SetType<int64_t> int64_set(int64_type);
const Col<Foo, std::set<int64_t>> set_col("set",
					  int64_set,
					  &Foo::fset); 

void table_insert_tests() {
  db::Ctx ctx("testdb/");
  Table<Foo> tbl(ctx, "insert_tests", {&uid_col},
		 {&int64_col, &str_col, &time_col});
  open(tbl);
  Foo foo;
  Trans trans(ctx);
  assert(insert(tbl, foo));
  assert(load(tbl, foo));
  assert(!insert(tbl, foo));
  commit(trans);
  assert(load(tbl, foo));
  close(tbl);
}

static void table_slurp_tests() {
  db::Ctx ctx("testdb/");
  Table<Foo> tbl(ctx, "slurp_tests", {&uid_col},
		 {&int64_col, &str_col, &time_col});
  open(tbl);
  
  Foo foo, bar;
  Trans trans(ctx);
  assert(insert(tbl, foo));
  assert(insert(tbl, bar));
  commit(trans);
  
  tbl.recs.clear();
  slurp(tbl);
  
  assert(load(tbl, foo));
  assert(load(tbl, bar));
  close(tbl);
}

static void read_write_tests() {
  db::Ctx ctx("testdb/");
  Table<Foo> tbl(ctx, "read_write_tests", {&uid_col},
		 {&int64_col, &str_col, &time_col, &set_col});
  open(tbl);
  
  crypt::Secret sec;
  init(sec, "secret key");

  Foo foo;
  foo.fint64 = 42;
  foo.fstr = "abc";
  foo.ftime = now();
  for (int i = 0; i < 100; i++) { foo.fset.insert(i); }
  
  Rec<Foo> rec;
  copy(tbl, rec, foo);
  
  Stream buf;
  write(tbl, rec, buf, sec);
  Rec<Foo> rrec;
  read(tbl, buf, rrec, sec);
  assert(compare(tbl, rrec, rec) == 0);
  
  close(tbl);
}

static void email_tests() {
  TRACE("Running email_tests");
  snackis::Ctx ctx("testdb/");
  ctx.db.inbox.recs.clear();
  
  Imap imap(ctx);
  fetch(imap);
}

int main() {
  std::cout << "Snackis v" << version_str() << std::endl;

  try {
    str_tests();
    fmt_tests();
    crypt_secret_tests();
    crypt_key_tests();
    schema_tests();
    table_insert_tests();
    table_slurp_tests();
    read_write_tests();
    email_tests();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  
  return 0;
}
