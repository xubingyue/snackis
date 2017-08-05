# Snabel
#### A flexible, statically typed postfix language embedded in C++

### Postfix
Like the scientific calculators of ancient times, and most printers in active use; but unlike most popular programming languages; Snabel expects arguments before operations.

```
> 3 4 35 + 10 %
2::I64
```

### Expressions
Snabel supports dividing expressions into parts using parentheses, each level starts with a fresh stack and the result is pushed to the outer stack on exit. Using braces instead of parentheses pushes the expression as is to the outer stack for later evaluation.

```
> (1 2 +) (2 2 *) +
7::I64

> {1 2 +}
n/a::OpSeq
```