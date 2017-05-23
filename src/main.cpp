#include <iostream>

#include "snackis/snackis.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/string_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/table.hpp"
#include "snackis/net/imap.hpp"

using namespace snackis;
using namespace snackis::db;

struct Foo {
  int64_t fint64;
  std::string fstring;
  Time ftime;
  UId fuid;

  Foo(): fint64(0), ftime(now()), fuid(uid()) { }
};

void col_tests() {
  const Col<Foo, std::string> col("string", string_type, &Foo::fstring); 

  Foo foo;
  foo.fstring = "abc";
  assert(col.get(foo) == "abc");
  col.set(foo, "def");
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

void table_tests() {
  Ctx ctx("testdb/");
  const Col<Foo, int64_t> int64_col("int64", int64_type, &Foo::fint64); 
  const Col<Foo, std::string> string_col("string", string_type, &Foo::fstring); 
  const Col<Foo, Time> time_col("time", time_type, &Foo::ftime); 
  const Col<Foo, UId> uid_col("uid", uid_type, &Foo::fuid); 
  Table<Foo> tbl(ctx, "foos", {&uid_col}, {&int64_col, &string_col, &time_col});
  open(tbl);
  Foo foo;
  assert(insert(tbl, foo));
  assert(!insert(tbl, foo));
  commit(ctx);
  close(tbl);
}

void email_tests() {
  using namespace snackis;

  Imap imap("imap.gmail.com", 993, "", "");
  std::vector<std::string> msgs;
  fetch(imap, msgs);

  for (auto m: msgs) {
    std::cout << "msg: " << m << std::endl;
  }
}

int main() {
  std::cout << "Snackis v" << snackis::version_string() << std::endl;

  try {
    col_tests();
    schema_tests();
    table_tests();
    email_tests();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  
  return 0;
}
