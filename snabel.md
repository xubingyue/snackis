# Snabel
#### A statically typed scripting-language embedded in C++

![script example](images/script.png?raw=true)

### Postfix
Just like Yoda of Star-Wars fame, and yesterdays scientific calculators; as well as most printers in active use; yet unlike currently trending programming languages; Snabel expects arguments before operations.

```
> 7 42 + 10 %
9::I64
```

### Expressions
Snabel supports dividing expressions into parts using parentheses, each level starts with a fresh stack and the last result is pushed on the outer stack.

```
> (1 2 +) (2 2 *) +
7::I64
```

### Lambdas
Using braces instead of parentheses pushes a pointer to the compiled expression on the stack, ```begin```/```end``` may be used to perform the same operation over multiple lines.

```
> {1 2 +}
n/a::Lambda

> {1 2 +} call
3::I64

> begin
    1
    2  +
    14 *
  end call
42::I64
```

### Bindings
Snabel supports named bindings using the ```let```-keyword. Bound identifiers are lexically scoped, and never change their value once bound in a specific scope. Semicolons may be used to separate bindings from surrounding code within a line.

```
> let fn {7 +}; 35 fn call
42::I64
```

### Types
Snabel provides static types; and will check and optimize code based on types of variables and functions during compilation. First class types and type-inference are provided to help reduce cognitive load and keyboard wear.

```
> I64
I64::Type
```

### Running the code
Snabel is designed to eventually be compiled as a standalone library, but the code is currently being developed inside [Snackis](https://github.com/andreas-gone-wild/snackis). The fastest way to try out the examples in this document is to first get Snackis up and running, and then execute 'script-new' to open the script view.