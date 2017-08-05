# Snabel
#### A statically typed scripting-language embedded in C++

### Postfix
Just like Yoda of Star-Wars fame, and scientific calculators of ancient times; as well as most printers in active use; but unlike currently trending programming languages; Snabel expects arguments before operations.

```
> 7 42 + 42 %
7::I64
```

### Expressions
Snabel supports dividing expressions into parts using parentheses, each level starts with a fresh stack and the last result is pushed on the outer stack.

```
> (1 2 +) (2 2 *) +
7::I64
```

### Lambdas
Using braces instead of parentheses pushes a pointer to the compiled expression on the stack for later evaluation.

```
> {1 2 +}
n/a::Lambda

> 1 {2 +} call
3::I64
```

### Bindings
Snabel supports named bindings using the ```let```-keyword. Bound identifiers are lexically scoped, and never change their value once bound in a specific scope. Semicolons may be used to separate bindings from surrounding code within a line.

```
> let fn {7 +}; 35 fn call
42::I64
```