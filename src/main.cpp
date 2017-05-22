#include "snackis/core/int64_type.hpp"
#include "snackis/core/string_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/table.hpp"

using namespace std;
using namespace snackis;

struct Foo {
  int64_t fint64;
  string fstring;
  UId fuid;

  Foo(): fint64(0), fuid(uid()) { }
};

void col_tests() {
  const Col<Foo, string> col("string", string_type, &Foo::fstring); 

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
  const Col<Foo, string> string_col("string", string_type, &Foo::fstring); 
  const Col<Foo, UId> uid_col("uid", uid_type, &Foo::fuid); 
  Table<Foo> tbl(ctx, "foos", {&uid_col}, {&int64_col, &string_col});
  open(tbl);
  Foo foo;
  assert(insert(tbl, foo));
  assert(!insert(tbl, foo));
  close(tbl);
}

int main() {
  col_tests();
  schema_tests();
  table_tests();
  return 0;
}
