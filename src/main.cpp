#include <iostream>

#include "snackis/snackis.hpp"
#include "snackis/core/buf.hpp"
#include "snackis/core/data.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/crypt/key.hpp"
#include "snackis/crypt/secret.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/table.hpp"
#include "snackis/net/imap.hpp"

using namespace snackis;
using namespace snackis::db;

void crypt_secret_tests() {
  using namespace snackis::crypt;
  str key("secret key");
  Secret sec;
  init(sec, key);
  str msg("secret message");
  
  Data cmsg(encrypt(sec, (const unsigned char *)msg.c_str(), msg.size())),
    dmsg(decrypt(sec, &cmsg[0], cmsg.size()));

  assert(str(dmsg.begin(), dmsg.end()) == msg);
}

void crypt_key_tests() {
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
  str fstring;
  Time ftime;
  UId fuid;

  Foo(): fint64(0), ftime(now()), fuid(uid()) { }

  Foo(db::Table<Foo> &tbl, db::Rec<Foo> &rec) {
    copy(tbl, *this, rec);
  }
};

void col_tests() {
  const Col<Foo, str> col("string", str_type, &Foo::fstring); 

  Foo foo;
  foo.fstring = "abc";
  assert(col.getter(foo) == "abc");
  col.setter(foo, "def");
  assert(foo.fstring == "def");
}

void schema_tests() {
  const Col<Foo, int64_t> col("int64", int64_type, &Foo::fint64); 
  Schema<Foo> scm({&col});

  Rec<Foo> foo, bar;
  foo[&col] = 42;
  assert(compare(scm, foo, bar) == 1);

  bar[&col] = 42;
  assert(compare(scm, foo, bar) == 0);
  
  bar[&col] = 43;
  assert(compare(scm, foo, bar) == -1);
}

const Col<Foo, int64_t> int64_col("int64", int64_type, &Foo::fint64); 
const Col<Foo, str> string_col("string", str_type, &Foo::fstring); 
const Col<Foo, Time> time_col("time", time_type, &Foo::ftime); 
const Col<Foo, UId> uid_col("uid", uid_type, &Foo::fuid); 

void table_tests() {
  Ctx ctx("testdb/");
  Table<Foo> tbl(ctx, "table_tests", {&uid_col},
		 {&int64_col, &string_col, &time_col});
  open(tbl);
  Foo foo;
  assert(insert(tbl, foo));
  assert(!insert(tbl, foo));
  commit(ctx);
  close(tbl);
}

void read_write_tests() {
  Ctx ctx("testdb/");
  Table<Foo> tbl(ctx, "read_write_tests", {&uid_col},
		 {&int64_col, &string_col, &time_col});
  open(tbl);
  
  crypt::Secret sec;
  init(sec, "secret key");

  Rec<Foo> rec;
  rec[&int64_col] = 42;
  rec[&string_col] = str("abc");
  rec[&time_col] = now();
  rec[&uid_col] = uid();

  Buf buf;
  write(tbl, rec, buf, &sec);
  Rec<Foo> rrec;
  read(tbl, buf, rrec, &sec);
  assert(compare(tbl, rrec, rec) == 0);
  
  close(tbl);
}

void email_tests() {
  TRACE("Running email_tests");

  Imap imap("imap.gmail.com", 993, "", "");
  std::vector<str> msgs;
  fetch(imap, msgs);

  for (auto m: msgs) {
    std::cout << "msg: " << m << std::endl;
  }
}

int main() {
  std::cout << "Snackis v" << version_str() << std::endl;

  try {
    crypt_secret_tests();
    crypt_key_tests();
    col_tests();
    schema_tests();
    table_tests();
    read_write_tests();
    email_tests();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  
  return 0;
}
