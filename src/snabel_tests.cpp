#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/exec.hpp"
#include "snabel/op.hpp"
#include "snabel/parser.hpp"
#include "snabel/type.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  static void test_func(Scope &scp, FuncImp &fn) {
    auto args(get_args(fn, scp));

    Exec &exe(scp.coro.exec);
    CHECK(args.size() == 1, _);
    CHECK(&args[0].type == &exe.i64_type, _);
    push(scp.coro, exe.i64_type, 42-get<int64_t>(args[0]));
  } 
  
  static void func_tests() {
    TRY(try_test);
    Exec exe;
    Coro &cor(exe.main);
    
    Func f("test-func");
    add_imp(f, {&exe.i64_type}, test_func);

    push(cor, Op::make_push(Box(exe.i64_type, int64_t(7))));
    push(cor, Op::make_call(f));
    run(exe);

    CHECK(get<int64_t>(pop(cor)) == 35, _);
  }

  static void parse_lines_tests() {
    TRY(try_test);    
    auto ls(parse_lines("foo\nbar\nbaz"));
    CHECK(ls.size() == 3, _);
    CHECK(ls[0] == "foo", _);
    CHECK(ls[1] == "bar", _);
    CHECK(ls[2] == "baz", _);
  }

  static void parse_backslash_tests() {
    TRY(try_test);    
    auto ls(parse_lines("foo\\\nbar\nbaz"));
    CHECK(ls.size() == 2, _);
    CHECK(ls[0] == "foo\\\nbar", _);
    CHECK(ls[1] == "baz", _);
    
    auto es(parse_exprs(ls[0]));
    CHECK(es.size() == 1, _);
    CHECK(es[0].text == "foo\\\nbar", _);
    
    auto ts(parse_expr(es[0]));
    CHECK(ts.size() == 2, _);
    CHECK(ts[0].text == "foo", _);
    CHECK(ts[1].text == "bar", _);
  }

  static void parse_semicolon_tests() {
    TRY(try_test);    
    auto es(parse_exprs("foo;bar"));
    CHECK(es.size() == 2, _);
    CHECK(es[0].text == "foo", _);
    CHECK(es[1].text == "bar", _);
  }

  static void parens_tests() {
    TRY(try_test);
    
    auto ts(parse_expr(Expr("foo (bar (35 7)) baz")));
    CHECK(ts.size() == 3, _);
    CHECK(ts[0].text == "foo", _);
    CHECK(ts[1].text == "(bar (35 7))", _);
    CHECK(ts[2].text == "baz", _);

    Exec exe;
    Scope &scp(get_scope(exe.main));
    compile(exe, "(1 1 +) (2 2 +) *");
    run(exe);
    CHECK(get<int64_t>(pop(scp.coro)) == 8, _);    
  }

  static void parse_tests() {
    parse_lines_tests();
    parse_backslash_tests();
    parse_semicolon_tests();
  }

  static void compile_tests() {
    TRY(try_test);
    Exec exe;
    Scope &scp(get_scope(exe.main));
    compile(exe, "let foo 35\nlet bar foo 14 -7 +");
    run(exe);
    CHECK(get<int64_t>(get_env(scp, "foo")) == 35, _);
    //CHECK(get<int64_t>(get_env(scp, "bar")) == 42, _);
  }

  static void stack_tests() {
    TRY(try_test);    
    Exec exe;
    Scope &scp(get_scope(exe.main));
    compile(exe, "42 reset 2 stash 3 4 + apply *");
    run(exe);
    CHECK(get<int64_t>(pop(scp.coro)) == 14, _);
  }

  static void scope_tests() {
    TRY(try_test);    
    Exec exe;
    Scope &scp(get_scope(exe.main));
    
    compile(exe, "(let foo 21;foo)");
    run(exe);
    CHECK(get<int64_t>(pop(scp.coro)) == 21, _);
    CHECK(!find_env(scp, "foo"), _);

    compile(exe, "begin\nlet bar 42\nbar\nend");
    run(exe);
    CHECK(get<int64_t>(pop(scp.coro)) == 42, _);
    CHECK(!find_env(scp, "bar"), _);

  }

  void all_tests() {
    func_tests();
    parse_tests();
    parens_tests();
    compile_tests();
    stack_tests();
    scope_tests();
  }
}
