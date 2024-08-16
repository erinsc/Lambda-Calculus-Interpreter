## What is the Lambda Calculus?
Great question! Imagine it's the 1930's and you're a logician trying to figure out what math is. As in, how much do you need to be able to do all of math. Turns out, not much!

The Lambda calculus has only 5 things in it, 3construction and 2 reduction rules. Let's go over the constructions first, we use them to construct lambda terms:
1. Variables. those can be anything really, numbers, characters, the value itself doesn't really matter. these are our base terms.
2. Abstractions. They're like the functions in programming, they have one input, represented by a variable, and a body which is some lambda term.
3. Applications. They're made up of two terms, a function and an argument.

Let's look at some examples:
- `x`,`y`,`z` are simple Variables.
- `\x.y` is an Abstraction with `x` as its input and `y` as its body.
- `x y` is an Application where `x` is the function and `y` is the argument.
- `(\a.(b c)) d` this one's a bit more complicated, it's an application with an abstraction as its function, and the abstraction has an application as its body.

Cool so now we can build lambda terms, let's look at a way we can reduce them.

1. Beta reduction. It might feel weird that we're starting with the second operation but it's actually the more important one. It works basically like normal function application: We find an abstraction, insert its argument, and replace all instances of the bound variable with copies of it.

Ok maybe that wasn't super clear. Let's look at an example:

Let's take a normal math function like `f(x) = x + 2`. `f` is the abstraction, `x` is its bound variable, and `x + 2` is its body. We can now calculate something like `f(5)`. Here, `5` is the argument for `f`. To calculate it we take the `5`, and replace all `x`'s inside `f`'s body with `5`, and we get `5 + 2`.

Beta reduction works the same. Take the lambda term `(\x.(x y)) z`. `z` is the argument, `\x.(x y)` the abstraction, `x` its bound variable. To beta reduce it we replace all `x`'s inside the body with `z`'s, and get `z y`. Easy!

A more complicated example, how would you reduce this term?

`((\x.(\y.(y x))) h) (\z.z)`

Remember we have to find an application which has an abstraction as its function. we can't reduce the topmost application `((\x.(\y.(y x))) h) (\z.z)` because its function is also an application `(\x.(\y.(y x))) h`, but we can reduce that application! its body is `(\y.(y x))` and we replace all `x`'s with `h`, and get back `(\y.(y h))`. The entire term is now `(\y.(y h)) (\z.z)`.

In the second step we replace all `y`'s with `(\z.z)`, and out term is now `(\z.z) h`.

In the last step we replace all `z`'s with `h`'s, and get `h`.

2. Alpha reduction. It's not really an operation, more a guideline. You might've noticed we don't really care what the variable names are, we just care which variables inside a body are bound and which aren't, so we can rename them if we want, or need to.

Take the term `(\x.(x x)) (\y.y)`. If we beta reduce it like normal, we'd get two different abstractions both with `y` as their bound variable, which could get confusing. To avoid it we can just rename one of them to `a`'s, and get `(\y.y) (\a.a)`.

Now a small detail, the way we've been writing the terms is a bit anoying, so we'll simplyfy it a bit, get rid of the brackets:

- We'll write `(a b) c` as just `a b c`, assume brackets from the left.
- `(\a.(b c))` as `(\a.b c)`, and
- `(\a.(\b.c))` as `(\a b.c)`. This last one is pretty interesting because its basically a 2-input abstraction, which we didn't have before.

## I mean, that's cool but how is this math?
 Oh, right! You might've noticed that there aren't any numbers, or even values. There's no way to store data. That's okay tho, we can simulate them using functions:

 Let's start with bools, true and false. What do we use them for normally? for if-else statements of course! We have two possible values we want to choose between. if we have a true, we want to get the first of the two, if we have a false we want the second. so we want an abstraction that takes two things and returns the first/second. We can define true as `\x y.x` and false as `\x y.y`.

 Now, take an abstraction like `\b.b X Y`. If we input true we get `X`, and if we input false we get `Y`. we've built an if-statement. Now imagine if `X` was false and `Y` true. We've just built a not-gate!

 It's not too hard to imagine how an and-gate will look like, it just takes two booleans instead of one. If the first is false, we return false, otherwise, if the second is true we return true, if its false we return false. I'll write `False` and `True` instead of the actual lambda terms to make it easier to read:

 `\a b.a (b True False) False`

 Now, you might've noticed that the second part was redundant, instead of using the second argument to decide we can just return it, so we can simplify our term to  `\a b.a b False`. But! if `a` is false, then there's no need to return a brand new false, we can just return `a`, so we can simplify our term to `\a b.a b a`, how cool is that?!
