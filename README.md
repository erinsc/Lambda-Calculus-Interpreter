# Lambda Calculus Interpreter
Primitive but fast hopefully. First time making a C project so bear with me.
If you don't know what the Lambda Calculus is check out "What is the Lambda Calculus.md".

## Representation of lambda terms
### In the console
- Variables represented using integers >= 2. Eventually lower-case characters will be supported for input too.
- Applications use implicit left brackets, `a b c` = `(a b) c`.
- Abstractions use `\` instead of `λ` for input. `\x.x` = `λx.x`. Chaining abstractions is supported: `\a b.b` = `\a.\b.b`

Example input:
```
> (\a b.a) (\x.x) u
(λa b.a) (λx.x) u
(λb x.x) u
λx.x
```

### In the program
A term is stored in an array as a binary tree. `0`s indicate Applications, `1`s Abstractions, `2`s Expressions.
- `(\b c.c b)` is represented as `{1,4,1,5,0,5,4}`.
- `(\a b.a) (\x.x) u` is represented as `{0, 0, 1, 3, 1, 4, 3, 1, 26, 26, 23}`.
