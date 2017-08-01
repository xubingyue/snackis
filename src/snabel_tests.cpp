#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/exec.hpp"
#include "snabel/op.hpp"
#include "snabel/parser.hpp"
#include "snabel/type.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  static void add(Ctx &ctx, FuncImp &fn) {
    Exec &exe(ctx.coro.exec);
    int64_t res(0);

    for (auto &a: ctx.coro.stack) {
      CHECK(&a.type == &exe.i64_type, _);
      res += get<int64_t>(a);
    }
    
    push(ctx.coro, exe.i64_type, res);
  }
  
  static void func_tests() {
    TRY(try_test);
    
    Exec exe;
    Ctx &ctx(get_ctx(exe.main));    
    Func &f(add_func(ctx, "+"));
    add_imp(f, {&exe.i64_type.seq}, add);
    
    run(ctx,
	{Push(exe.i64_type, int64_t(35)),
	    Push(exe.i64_type, int64_t(7)),
	    Call(f),
	    Let("foo")});
    
    CHECK(get<int64_t>(get_env(ctx, "foo")) == 42, _);
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
    
    auto es(parse_line(ls[0]));
    CHECK(es.size() == 1, _);
    CHECK(es[0].text == "foo\\\nbar", _);
    
    auto ts(parse_expr(es[0]));
    CHECK(ts.size() == 2, _);
    CHECK(ts[0].text == "foo", _);
    CHECK(ts[1].text == "bar", _);
  }

  static void parse_semicolon_tests() {
    TRY(try_test);    
    auto es(parse_line("foo;bar"));
    CHECK(es.size() == 2, _);
    CHECK(es[0].text == "foo", _);
    CHECK(es[1].text == "bar", _);
  }

  static void parse_tests() {
    parse_lines_tests();
    parse_backslash_tests();
    parse_semicolon_tests();
  }

  static void compile_tests() {
    TRY(try_test);    
    Exec exe;
    Ctx &ctx(get_ctx(exe.main));
    Func &f(add_func(ctx, "+"));
    add_imp(f, {&exe.i64_type.seq}, add);
    OpSeq ops(compile(ctx, "let foo 35\nlet bar foo 14 -7 +"));
    run(ctx, ops);
    CHECK(get<int64_t>(get_env(ctx, "foo")) == 35, _);
    CHECK(get<int64_t>(get_env(ctx, "bar")) == 42, _);
  }

  void all_tests() {
    func_tests();
    parse_tests();
    compile_tests();
  }
}
