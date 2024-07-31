# Lambda Calculus Interpreter
Primitive but fast hopefully. First time making a C project so bear with me.

## What is the Lambda Calculus?
It's like a programming language but there's only 3 things: abstractions, applications, and variables. You can build a binary tree out of these, a 'Lambda term'; applications branch into two other terms, left is called its 'function', the right its 'argument'. abstractions branch into a variable, the 'bound variable', and a term called its 'body'. variables are leaves of the tree.

There are only two operations you can do on the terms, and only the second one actually does something. Alpha reduction renames variables to avoid variable conflict: we don't care about the names themselves, just the relation between them and their bound variable. To avoid confusion we change variable names so every variable is bound only once.

Beta reduction is the main workhorse here. It is only usable on an application whose function is an abstraction: We find all variables with the same name as the bound variable of the function, and replace them with copies of the argument (renaming them with alpha reduction to avoid conflicts).

- Variables can look like anything: `a`, `x`, `hello`, `36`.
- Abstractions look like this: `λ<bound variable>.<body>`, for example `λx.x`, `λx.y`.
  - If the body is an abstraction too you can smush them together for better readability: `λx.λy.x` = `λx y.x`.
  - If you can't type `λ` you can use `\`.
- Applications look like this: `<function> <argument>`, using brackets to avoid ambiguity. `(a b) c` vs `a (b c)`.
  - You can imply brackets from the left, again to make it easier to read: `a b c` = `(a b) c`.

### Representation in the program is a bit different tho

The binary tree is written into an array as a sequence of numbers. 0 denotes an application, 1 an abstraction, and all other numbers a variable. `{0,1,2,3,0,4,5}` == `(λ2.3) (4 5)`.

## Wait so what is all this for?
With enough effort you can represent any calculation you want with just these terms. You can think of them like the building blocks of math and programming, which is pretty fun!

Let's start with some building blocks first:

- `λx.x`, I or Ibis, takes an argument and returns it, simple as.
  - `(λx.x) hello` -> `hello`
  - `(λx.x) (λy.y) world` ->
    `(λy.y) world` ->
    `world`
- `λx y.x`, K or Kestrel. takes two arguments and returns the first.
  - `(λx y.x) hello world` ->
    `(λy.hello) world` ->
    `hello`
- `λx y.y`, KI or Kite. takes two arguments and returns the second.
  - It might not be obvious at first but as the name implies, `KI` = `λx y.x`:
  - `(λx y.x) (λz.z)` ->
    `λy.(λz.z)` = `λy z.z`. Again the variable names dont matter just their relative positions to each other and their bound variable.
- `λx y z.x(y z)`, B or Bluebird, looks kinda complicated since it actually does something but it's just function composition. it takes two terms and aplies them one after another to an argument.
  - `(λx y z.x(y z)) (λx y.x) (λx.x) hatsune miku` ->
    `(λy z.(λx w.x)(y z)) (λx.x) hatsune miku` ->
    `(λy z w.(y z)) (λx.x) hatsune miku` ->
    `(λz w.((λx.x) z)) hatsune miku` ->
    `(λz w.z) hatsune miku` ->
    `(λw.hatsune) miku` ->
    `hatsune`.
- `λx y z.xzy`, C or Cardinal, reverses the first two arguments for a function.
- `λx y z.xz(zy)`, S or Starling, it's like C and B at the same time.

## I mean cool but you said there'd be math, this isn't math

I'll finish this later, got bored writing lol
