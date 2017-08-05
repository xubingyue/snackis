#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/exec.hpp"
#include "snabel/op.hpp"
#include "snabel/parser.hpp"
#include "snabel/type.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  static void test_func(Scope &scp, FuncImp &fn, const Args &args) {
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
    add_imp(f, {&exe.i64_type}, exe.i64_type, test_func);

    cor.ops.push_back(Op::make_push(Box(exe.i64_type, int64_t(7))));
    cor.ops.push_back(Op::make_call(f));
    run(exe.main);

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
    
    auto ts(parse_expr(ls[0]));
    CHECK(ts.size() == 2, _);
    CHECK(ts[0].text == "foo", _);
    CHECK(ts[1].text == "bar", _);
  }

  static void parse_string_tests() {
    auto ts(parse_expr("\"foo\" 1 +"));
    CHECK(ts.size() == 3, _);
    CHECK(ts[0].text == "\"foo\"", _);
    CHECK(ts[1].text == "1", _);
    CHECK(ts[2].text == "+", _);

    ts = parse_expr("1 \"foo\" +");
    CHECK(ts.size() == 3, _);
    CHECK(ts[0].text == "1", _);
    CHECK(ts[1].text == "\"foo\"", _);
    CHECK(ts[2].text == "+", _);
  }
  
  static void parse_tests() {
    parse_lines_tests();
    parse_backslash_tests();
    parse_string_tests();
  }

  static void parens_tests() {
    TRY(try_test);
    
    auto ts(parse_expr("foo (bar (35 7)) baz"));
    CHECK(ts.size() == 3, _);
    CHECK(ts[0].text == "foo", _);
    CHECK(ts[1].text == "(bar (35 7))", _);
    CHECK(ts[2].text == "baz", _);

    Exec exe;
    Scope &scp(curr_scope(exe.main));
    compile(exe.main, "(1 1 +) (2 2 +) *");
    run(exe.main);
    CHECK(get<int64_t>(pop(scp.coro)) == 8, _);    
  }

  static void compile_tests() {
    TRY(try_test);
    Exec exe;
    Scope &scp(curr_scope(exe.main));
    compile(exe.main, "let foo 35\nlet bar foo 14 -7 +");
    run(exe.main);
    CHECK(get<int64_t>(get_env(scp, "foo")) == 35, _);
    CHECK(get<int64_t>(get_env(scp, "bar")) == 42, _);
  }

  static void stack_tests() {
    TRY(try_test);    
    Exec exe;
    Scope &scp(curr_scope(exe.main));
    compile(exe.main, "42 reset");
    run(exe.main);
    CHECK(!peek(scp.coro), _);
  }

  static void scope_tests() {
    TRY(try_test);    
    Exec exe;
    Scope &scp(curr_scope(exe.main));
    
    compile(exe.main, "(let foo 21;foo)");
    run(exe.main);
    CHECK(get<int64_t>(pop(scp.coro)) == 21, _);
    CHECK(!find_env(scp, "foo"), _);

    compile(exe.main, "begin\nlet bar 42\nbar\nend");
    run(exe.main);
    CHECK(get<int64_t>(pop(scp.coro)) == 42, _);
    CHECK(!find_env(scp, "bar"), _);
  }

  static void jump_tests() {
    TRY(try_test);    
    Exec exe;
    //Scope &scp(curr_scope(exe.main));
    compile(exe.main, "42 !exit 7 @exit +");
    run(exe.main);
    //CHECK(get<int64_t>(pop(scp.coro)) == 42, _);
  }

  void all_tests() {
    func_tests();
    parse_tests();
    parens_tests();
    compile_tests();
    stack_tests();
    scope_tests();
    jump_tests();
  }
}
