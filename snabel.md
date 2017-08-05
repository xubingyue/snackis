# Snabel
#### A flexible, statically typed language embedded in C++

### Postfix
Like the scientific calculators of ancient times, and most printers in active use; but unlike most popular programming languages; Snabel expects arguments before operations.

```
> 7 42 + 42 %
7::I64
```

### Expressions
Snabel supports dividing expressions into parts using parentheses, each level starts with a fresh stack and the result is pushed to the outer stack on exit. 

```
> (1 2 +) (2 2 *) +
7::I64
```

### Lambdas
Using braces instead of parentheses pushes the compiled expression to the stack for later evaluation.

```
> {1 2 +}
n/a::Lambda

> 1 {2 +} call
3::I64
```


### Bindings
Snabel supports introducing named bindings using the ```let```-keyword. Bindings are lexically scoped, and the value for a specific name is not allowed to change once bound. ```;``` may be used to separate bindings from surrounding code without adding line-breaks.

```
> let fn {7 +}; 35 fn call
42::I64
```