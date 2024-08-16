# Lambda Calculus Interpreter
Primitive but fast hopefully. First time making a C project so bear with me.
If you don't know what the Lambda Calculus is check out "What is the Lambda Calculus.md".

## Representation of lambda terms
### In the console
Variables are represented using lower-case characters optionally followed by an integer; `x`, `y1`, `a34`.

Applications use implicit left brackets and are printed without spaces; `abc` = `(ab)c`.

Abstractions use `\` instead of the lambda character for input and output, chained abstractions are concatenated; `\xy.x` = `λx.λy.x`.

Interpreter supports aliases for terms, represented with a chain of upper-case characters up to 6 long. Aliases may be used as constants in other terms. While reducing term the interpreter may be set to unpack aliases when no beta applications may be performed. Alpha-reduction is automatically performed on the unpacked alias to avoid variable collision.

The interpreter also performs alpha-reduction to minimize variable names, which may result in variables being renamed unfavorably. This is deemed a feature as it reminds that the structure of terms is important rather than its names.

Example session, with added comments after `//`:
```
> \xy.x
\xy.x   // Parsed input
\ab.a   // Alpha-reduction minimized variables

> = TRUE // Term defined as TRUE
TRUE = \ab.a // Confirmation of the definition

> \xy.y
\xy.y
\ab.b

> = FALSE
FALSE = \ab.b

> \x.x FALSE TRUE // Term which includes aliases inside
\x.x FALSE TRUE   // Unpacking of aliases is enabled by default
\x.x(\yz.z) TRUE
\x.x(\yz.z)(\a1b1.a1)
\a.a(\bc.c)(\de.d) // Completely unpacked and reduced term

> = NOT
NOT = \a.a(\bc.c)(\de.d)

> -d // Toggles alias unpacking

> \x.x FALSE TRUE
\x.x FALSE TRUE
\a.a FALSE TRUE // Same input, this time not unpacked

>
```

### In the program
A term is stored in an array as a binary tree.
- 0s indicate Applications, and are followed by the function then argument.
- 1s indicate Abstractions, are followed by variable and body.
- 2s indicate Aliases, and are followed by a single number encoding the alias name.
- Other values indicate variables and encode the structure in a single number.

`(\b c.c b)` is represented as `{1,4,1,5,0,5,4}`.

`(\a b.a) (\x.x) u` is represented as `{0, 0, 1, 3, 1, 4, 3, 1, 26, 26, 23}`.
