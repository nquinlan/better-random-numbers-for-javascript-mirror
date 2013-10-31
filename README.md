#  Better Random Numbers for Javascript

_Note: This is a direct mirror of Johannes Baagøe's wiki on implementations of Randomness in Javascript. [Mr. Baagøe's site](http://baagoe.org/) has largely disappeared from the internet this is a faithful reproduction of it from [Archive.org](https://web.archive.org/web/20120124013936/http://baagoe.org/en/wiki/Better_random_numbers_for_javascript) and other sources. The content is Licensed [Creative Commons Attribution Share Alike](http://creativecommons.org/licenses/by-sa/3.0/)._


ECMAScript<a name="#cite_ref-0"></a><sup>[[1]](#cite_note-0)</sup> provides a standard function, `Math.random`, which according to the specifications ([ECMA-262, 3rd edition](http://www.ecma-international.org/publications/standards/Ecma-262-arch.htm), 15.8.2.14) "returns a number value with positive sign, greater than or
equal to 0 but less than 1, chosen randomly or pseudo randomly with approximately uniform distribution over that range, using an implementation-dependent algorithm or strategy. This function takes no arguments."

This is good enough for its intended use, viz., to provide an easy way for script authors to write cute animations, games, etc. However, if you want to use it for more serious programming, there are several problems.

Contents
--------

-   [1 Why Math.random should not be used for serious programming](#Why_Math.random_should_not_be_used_for_serious_programming)
    -   [1.1 No guarantee of quality](#No_guarantee_of_quality)
    -   [1.2 No standard way to repeat sequences](#No_standard_way_to_repeat_sequences)
    -   [1.3 Security](#Security)

-   [2 Why it is difficult to write good PRNGs in javascript](#Why_it_is_difficult_to_write_good_PRNGs_in_javascript)
    -   [2.1 Only 53 significant bits in multiplications](#Only_53_significant_bits_in_multiplications)
    -   [2.2 Limited and inefficient bit operations](#Limited_and_inefficient_bit_operations)

-   [3 A collection of better PRNGs](#A_collection_of_better_PRNGs)
    -   [3.1 Common interface](#Common_interface)
        -   [3.1.1 Simple usage](#Simple_usage)
        -   [3.1.2 Using methods and properties](#Using_methods_and_properties)

    -   [3.2 Common implementation details](#Common_implementation_details)
        -   [3.2.1 Mash](#Mash)
        -   [3.2.2 At least two random functions, not just one](#At_least_two_random_functions.2C_not_just_one)

    -   [3.3 The generators](#The_generators)
        -   [3.3.1 MRG32k3a](#MRG32k3a)
        -   [3.3.2 Xorshift03](#Xorshift03)
        -   [3.3.3 KISS07](#KISS07)
        -   [3.3.4 LFib](#LFib)
        -   [3.3.5 LFIB4](#LFIB4)
        -   [3.3.6 Alea](#Alea)
        -   [3.3.7 Kybos](#Kybos)

-   [4 License](#License)
-   [5 Notes](#Notes)

Why `Math.random` should not be used for serious programming
------------------------------------------------------------

### No guarantee of quality

"Using an implementation-dependent algorithm or strategy" means that you don't know how the "random" numbers are made and how "random" they are likely to be, even if you know a lot about [PRNG](http://en.wikipedia.org/wiki/PRNG)s. You have to inspect the code if it is [Open Source](http://en.wikipedia.org/wiki/Open_source), or trust the vendor otherwise.

### No standard way to repeat sequences

In most implementations if not in all, `Math.random`'s generator is silently seeded at startup by some supposedly random value, usually the
present time to the millisecond or whatever other granularity the system provides.

This means that there is no easy way to repeat a series of pseudo-random numbers in order, e.g., to determine what went wrong in a simulation. It
also means that you cannot give your code to a colleague and expect that she will find what you want to show. Anything that uses `Math.random` is inherently not portable.

### Security

On the other hand, the weakness of many implementations mean that a sequence of outputs of `Math.random` can often be predicted, even when
the intent was that it could not. This may present security risks. `Math.random` should never be used to generate separators in POST
requests<a name="#cite_ref-1"></a><sup>[[2]](#cite_note-1)</sup>, [Version 4 UUIDs](http://en.wikipedia.org/wiki/Universally_Unique_Identifier), [session keys](http://en.wikipedia.org/wiki/Session_key), etc.

If one wants to do things like that in javascript, `Math.random` is simply not good enough.

Why it is difficult to write good PRNGs in javascript
-----------------------------------------------------

Many families of excellent PRNGs have been conceived for other languages. However, javascript's notion of Numbers make most of them
less than ideal for our purpose. They can be ported (javascript, like any other decent programming language, is [Turing complete](http://en.wikipedia.org/wiki/Turing_completeness)), but at a heavy price in speed.

### Only 53 significant bits in multiplications

Many present algorithms assume exact multiplications on 32-bit unsigned integers, yielding a 64-bit result. Some even multiply 64-bit integers, yielding a 128-bit result. If you want to emulate that in javascript, you have to write multi-precision routines. That is certainly possible, but hardly efficient.

Even worse, most algorithms based on multiplications assume that overflow results in truncation and discard of the high bits, effectively giving modular arithmetic for free. This works fine on unsigned integers, but javascript Numbers are always "double-precision 64-bit binary format IEEE 754 values". In the case of overflow, it is the **low order** bits that are discarded.

As an example, let us examine the best-known PRNG of all: the venerable [LCG](http://en.wikipedia.org/wiki/Linear_congruential_generator), on 32-bit unsigned integers.

If you use [George Marsaglia](http://en.wikipedia.org/wiki/George_Marsaglia)'s "easy to remember" multiplier 69069, everything works nicely: 69069
being less than 2<sup>21</sup>, `x = 69069 * x + 1 >>> 0;` will indeed cycle
through all the possible values.

But if you choose a bigger multiplier modulo 2<sup>32</sup> with good figures of merit in the [spectral test](http://random.mat.sbg.ac.at/tests/theory/spectral/), say, 1103515245 which is used in ANSI C and the glibc library, the
result of the multiplication will almost never fit in 53 bits. Low order bits are discarded, which ruins the basis for the algorithm: its
operation modulo 2<sup>32</sup>.

### Limited and inefficient bit operations

Javascript has the usual set of bitwise operations: AND, OR, XOR, shifts, etc. However, they only apply to 32-bit integers, and since javascript Numbers are always floating point, it means that the Numbers must be converted internally to integers before the operation, and back to floats again when it is done. Smart implementations may attempt to optimise, but on all the platforms I have tried, the results remain slow.

Altogether, precious few of the usual tools are well adapted to javascript. Some ingenuity is required if one wants good performance.

A collection of better PRNGs
----------------------------

I present several PRNGs that should have much better statistical properties than the built-in `Math.random`. They have been taken from, or inspired by, well-known sources on the subject, and they have been extensively tested. They share a common interface, but the actual algorithms differ, so that one can choose the best for one's platform and needs. They can be downloaded [here](support/Alea-0.9.zip).

### Common interface

We shal take `Alea` as example, but the other PRNGs - `MRG32k3a`, `KISS07`, etc - share the same interface.

#### Simple usage

```javascript
  var random = [new] Alea([...]);
```

Calling `Alea` (or any of the others) with `new` is unnecessary, but harmless. The optional arguments may be of any number and nature.

The call returns a function, `random`, that is functionally equivalent to `Math.random`.

That is, successive calls of `random()` return Number values with positive sign, greater than or equal to 0 but less than 1, chosen pseudo-randomly with approximately uniform distribution over that range. (Hopefully, with a much better approximation of that [uniform distribution](http://en.wikipedia.org/wiki/Uniform_distribution_(continuous)).)

Example

```javascript
  var random = new Alea("my", 3, "seeds");
  random(); // returns 0.30802189325913787
  random(); // returns 0.5190450621303171
  random(); // returns 0.43635262292809784
```

Any implementation of ECMAScript should yield exactly those values.

The internal state of random, and hence the sequence of pseudo-random numbers it returns, is determined by the arguments to `Alea`. Two functions returned by calls to `Alea` with the same argument values will return exactly the same sequence of pseudo-random numbers. String and Number arguments should provide repeatable output across platforms. Object arguments<a name="#cite_ref-2"></a><sup>[[3]](#cite_note-2)</sup> provide repeatable output on the same platform, but not necessarily on others.

If you call `Alea()`, that is, with no arguments, a single argument of `+new Date()` is silently assumed. This provides an easy means to provide somewhat unpredictable numbers, like `Math.random` does.

Example:

```javascript
  var random = Alea();
  random(); // returns 0.6198398587293923
  random(); // returns 0.8385338634252548
  random(); // returns 0.3644848605617881
```

Your return values should be completely different. (But see below how you can reproduce mine on your computer.)

The generated numbers should be "more random" than in most implementations of `Math.random`. Any sequence of any pick of the returned bits should pass any known test, unless where mentioned. (Please send me a mail if you have evidence that this is not true. But please remember that "p's happen": if you repeat any test a million times, some of the results are bound to have very suspicious p-values.)

The specification of `Math.random` says nothing about the precision of the returned fraction. Here, it is guaranteed that the precision is at least 32 bits.

Very often, to get the full attainable precision of 53 bits, the main generator has to be called twice, which takes more time. It would be wasteful to insist on doing that systematically, since a 32 bit resolution is enough in most cases. If you really need 53 bits, the function provides a method described below to get them.

#### Using methods and properties

Functions being Objects in javascript, the returned function also has two methods, `uint32` and `fract53`, and two properties, `version` and
`args`.

`uint32` is a function that takes no arguments and returns an unsigned random integer in the range [0, 2<sup>32</sup>[.

Example:

```javascript
  var intRandom = Alea("").uint32;
  intRandom(); // returns 715789690
  intRandom(); // returns 2091287642
  intRandom(); // returns 486307
```

Any implementation of ECMAScript should yield exactly those values.

To obtain an integer in [0, n[, one may simply take the remainder modulo n<a name="#cite_ref-3"></a><sup>[[4]](#cite_note-3)</sup>. With some generators, this is faster than using the default function in `Math.floor(random() * n)` or its clever variants, `random() * n | 0` and `random() * n >>> 0`

`fract53` is a function that takes no arguments and returns a 53-bit fraction in [0, 1[. It is usually slower than the main function, though.

Example:

```javascript
  var rand53 = Alea("").fract53;
  rand53(); // returns 0.16665777435687268
  rand53(); // returns 0.00011322738143160205
  rand53(); // returns 0.17695781631176488
```

Any implementation of ECMAScript should yield exactly those values.

`version` is a String that indicates the nature and the version of the generator, like 'Alea 0.9'.

`args` is an Array of the arguments that were given to `Alea`. Thus, if you called it without an argument and it assumed
`+new Date()`, you may recover the time stamp that was used in order to repeat the sequence.

Example, assuming random is the function created above without arguments (I wrote the example on 22 June 2010 shortly after 7AM in France (summer time), exactly at 2010-06-22T07:01:18.230+02:00):

```javascript
  var seed = random.args; // seed = 1277182878230
```

This value can be used to reproduce the values in the earlier example that used `Alea()` without arguments:

```javascript
  random = Alea(1277182878230);
  random(); // returns 0.6198398587293923
  random(); // returns 0.8385338634252548
  random(); // returns 0.3644848605617881
```

Any implementation of ECMAScript should yield exactly those values.

### Common implementation details

Regardless of the differences in their algorithms, internal state, etc, all the generators provided here share a common implementation pattern.
First, the internal state is declared. Then, it is seeded. Then, the main function for the algorithm is presented. Finally, the function-object to be returned is built and returned.

#### Mash

[Javascript](support/js/Mash.js) | [C](support/c/mash.h)

The arguments to `Alea` (and the other provided generators) are always converted into a String which is hashed by the means of a suitable hash
function.

That hash function, with its initialised internal state, is provided by another function called [Mash](support/js/Mash.js), which must therefore be included in any application that uses any of the provided functions.

(Alternatively, if you decide to use only one PRNG, you may simply copy Mash into your copy of the relevant file.)

Each element of the internal state is altered by a hash of each argument. (The hash function preserves its own internal state, so that
each of the alterations is likely to be different.) The exact nature of the alteration depends on the nature of the state element - if it is an integer, the hash is simply added, if it is a single bit, it is XOR-ed, if it is a fraction between 0 and 1, the hash is subtracted and the element is incremented by 1 if it has become negative, etc.

Finally, whenever the PRNG algorithm places constraints on certain elements of its internal state, the last stage of the seeding process makes sure that these constraints are met.

#### At least two random functions, not just one

All the generators provide two functions, the main function which returns 32-bit or 53-bit fractions, and its `uint32` method which returns 32-bit unsigned integers. They also provide the `fract53` method which is guaranteed to return 53-bit fractions; it may or may not be the same as the main function.

In most algorithms ported from other languages, the algorithm is actually implemented in the `uint32` method. (Operating on 53-bit numbers without ever losing a bit to overflow leaves very few possibilities. 32-bit unsigned integers provide handy bit operations in addition to reducing the problems of unintended overflow; most PRNGs operate on integers for that reason.) The "main" container function calls that integer function and multiplies the result by 2<sup>32</sup>, the `fract53` method calls it twice, once for its 32 high bits and once again for the 21 lowest.

In the generators of the Lagged Fibonacci family, the PRNG algorithm directly provides the 53-bit fractions. The `uint32` method multiplies by 2<sup>32</sup> and returns the floored result. The `fract53` method is the same as the main container function.

In `Alea` and `Kybos`, the main PRNG algorithm provides 32-bit fractions. Both `uint32` and `fract53` are derived.

### The generators

#### MRG32k3a

[Javascript](support/js/MRG32k3a.js) | [C](support/c/MRG32k3a.c)

One of [Pierre L'Ecuyer](http://www.iro.umontreal.ca/~lecuyer/)'s Combined Multiple Recursive<a name="#cite_ref-4"></a><sup>[[5]](#cite_note-4)</sup> PRNGs. It is remarkable for its ingenious use of 53-bit integer arithmetic (originally implemented in C doubles) which makes it very suitable for javascript, as well as for the are in the choice of multipliers and moduli. The period is about 2<sup>191</sup>, and it passes - of course - L'Ecuyers own [TestU01](http://www.iro.montreal.ca/~simardr/testu01/tu01.html) BigCrush battery of tests. (The [Mersenne Twister](http://en.wikipedia.org/wiki/Mersenne_twister) fails Crush.)

This is surely the most reputable of all the generators presented here. It has been quite extensively tested and it is widely used. It is copyrighted but free for non-commercial uses. Commercial users must request written permission from Professor L'Ecuyer.

#### Xorshift03

[Javascript](support/js/Xorshift03.js) | [C](support/c/Xorshift03.c)

George Marsaglia's [2003 version](http://groups.google.com/group/comp.lang.c/msg/e3c4ea1169e463ae) of his xorshift family of PRNGs. They are among the fastest available in integer arithmetic, but they become much slower in javascript. Also, most variants tend to fail L'Ecuyer's tests which are rather more stringent than Marsaglia's own.

This version, however, passes BigCrush, mainly because it contains a final multiplication of two components. Its period is about 2<sup>160</sup>.

#### KISS07

[Javascript](support/js/KISS07.js) | [C](support/c/KISS07.c)

George Marsaglia's [2007 version](http://groups.google.com/group/comp.lang.fortran/msg/6edb8ad6ec5421a5) of his KISS family of PRNGs.

It is deceptively simple. It merely adds the outputs of three small generators which individually would not have the slightest chance of passing any kind of randomness test. Yet, the result passes BigCrush. The period is about 2<sup>121</sup>.

Its xorshift component makes it rather slow, though. It remains a very attractive generator, because its combination of three completely different algorithms with different periods makes it likely that any bias from one of them is blurred by at least one of the two others.

#### LFib

[Javascript](support/js/LFib.js) | [C](support/c/LFib.c)

This is a L(2<sup>53</sup>, 255, 52, -) [lagged Fibonacci](http://en.wikipedia.org/wiki/Lagged_Fibonacci_generator)
generator<a name="#cite_ref-5"></a><sup>[[6]](#cite_note-5)</sup>. The period is very close to 2<sup>307</sup>.

It uses subtraction modulo 1 instead of addition in order to retain full 53-bit precision. (One cannot add two 53-bit fractions in [0, 1[ in IEEE
double precision without risking the loss of a bit to overflow. One can subtract them, though, and add 1 if the result is negative - the sign flag serves as a temporary 54-th bit.)

`LFib` and `LFIB4` are the fastest of the generators presented here to provide full 53-bit resolution, since that is what they provide natively. However, the simple linear dependency between their last bits makes me somewhat reluctant to recommend them wholeheartedly, even though they pass BigCrush. They fail [Michael Niedermayer's tests](http://guru.multimedia.cx/pseudo-random-number-generators/). All of the other PRNGs presented here pass.

#### LFIB4

[Javascript](support/js/LFIB4.js) | [C](support/c/LFIB4.c)

Marsaglia's [LFIB4](http://groups.google.com/group/sci.crypt/msg/eb4ddde782b17051), adapted to subtraction modulo 1 on 53-bit fractions.

The period is very close to 2<sup>308</sup>, and its combination of four "taps" instead of two makes it more robust than the usual lagged Fibonacci generators. It also makes it just a little slower, although much faster in javascript than integer-based algorithms.

#### Alea

[Javascript](support/js/Alea.js) | [C](support/c/Alea.c)

This generator implements my variation on Marsaglia's [Multiply-with-carry](http://en.wikipedia.org/wiki/Multiply-with-carry)
theme, adapted to javascript's quaint notion of numbers: the carries are exactly the integer parts of Numbers with exactly 32 bits of fractional part.

Such generators depend crucially on their multiplier, *a*. It must be less than 2<sup>21</sup>, so that the result of the multiplication fits in 53 bits, and for an array of *n* 32-bit numbers, *a*2<sup>32*n*</sup> − 1 must be a
[safe prime](http://en.wikipedia.org/wiki/Safe_prime). The period is the corresponding [Germain prime](http://en.wikipedia.org/wiki/Sophie_Germain_prime), *a*2<sup>32*n*\\ −\\ 1</sup> − 1.

The one presented here uses *n* = 3: just three 32-bit fractions, which means that one may use three rotating variables without walking through an Array. (Table lookup is rather expensive, time-wise.) The period is close to 2<sup>116</sup>, it passes BigCrush, and it is the fastest javascript PRNG I know that does so. 

I expected such generators with any *n* up to 256 (or even beyond, if one wants monstrously long periods and can find the appropriate multipliers) to be faster than those relying on integer arithmetics, which they are. But they also turn out to be faster than the lagged Fibonacci generators if one does not insist on 53 bits, much to my surprise.

I therefore propose them as the PRNGs of choice in javascript. I must however confess to the bias induced by more personal involvement in those generators than in the others, which are simple ports in the case of `MRG32k3a`, `Xorshift03` and `KISS07`, and rather straightforward adaptations in the case of the lagged Fibonaccis.

#### Kybos

[Javascript](support/js/Kybos.js) | [C](support/c/Kybos.c)

All the generators presented so far have quite well-defined mathematical properties which have been extensively studied. This gives good reasons to consider them satisfactory, but it also raises a suspicion - aren't they **too** well-behaved? Isn't their elegant mathematical structure by itself a sign that they are not *random*? Except possibly for `KISS07` which combines three quite different ideas, they could be victims of the
Mersenne Twister syndrome - a theoretically wonderful algorithm, with actual proofs of good distribution in all dimensions up to a large number and a huge period, which turns out to fail tests that other, less sophisticated but more robust generators pass easily.

`Kybos`<a name="#cite_ref-6"></a><sup>[[7]](#cite_note-6)</sup> combines `Alea` with a variant of the Bays-Durham shuffle), using the ultimate non linear tool: table lookup. The original shuffle rarely improves PRNGs in a noticeable way, but the variant used here, in which the "random" numbers in the lookup table are incremented by those from the original generator instead of being replaced, makes some terrible generators pass very stringent tests.

The problem is that one cannot say much about the period or other aspects of the resulting numbers. Which may be what one would expect of
anything that is indeed random, but it doesn't make mathematicians happy.

I mainly propose `Kybos` because it has an extra method, `addNoise`, which allows one to alter the state of the lookup table using true, physical randomness, without changing anything in the state of the `Alea` generator that provides the guarantee of a suitably long period and good distribution. The lookup table essentially serves as an entropy pool. Its size of 256 bits makes it suitable for the generation of 128-bit UUIDs.

License
-------

`MRG32k3a` is [copyrighted](http://www.iro.umontreal.ca/~simardr/testu01/copyright.html) as a part of [TestU01](http://www.iro.umontreal.ca/~simardr/testu01/tu01.html). 

The status of `Xorshift03`, `KISS07` and `LFIB4` is somewhat unclear. The sources are Usenet posts, and one of them explicitly states ["You
are welcome to use one or both of the above"](http://groups.google.com/group/comp.lang.fortran/msg/6edb8ad6ec5421a5). It seems unlikely that Professor Marsaglia intended to restrict their use in any way, but on the other hand, I have no indication that he ever formally released them to the public domain.

My own work is licensed according to the [MIT - Expat license](http://en.wikipedia.org/wiki/MIT_License):

    Copyright (C) 2010 by Johannes Baagøe <baagoe@baagoe.org>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

Notes
-----

1.  <a name="cite_note-0"></a>[↑](#cite_ref-0)
    A note on terminology: I use "javascript" (lowercase, except when
    starting a sentence) as a deliberately loose generic term for all
    dialects of the language. "JavaScript" (CamelCase) designates
    Netscape / Mozilla / Gecko implementations, "JScript" designates
    Microsoft's,
    "[ECMAScript](http://en.wikipedia.org/wiki/ECMAScript)"
    implies conformance with a formally defined version. They are all
    javascript, though.
2.  <a name="cite_note-1"></a>[↑](#cite_ref-1)
    [Klein, Amit. 2008. *Temporary user tracking in major browsers and
    Cross-domain information leakage and
    attacks*](http://www.trusteer.com/sites/default/files/Temporary_User_Tracking_in_Major_Browsers.pdf)
3.  <a name="cite_note-2"></a>[↑](#cite_ref-2)
    The point of using Object arguments to initialise the generators
    would be to provide thoroughly unpredictable output. As an extreme
    example, one could try `Alea(document)`
    or `Alea(this)` in a browser
4.  <a name="cite_note-3"></a>[↑](#cite_ref-3)
    This should only be done in the common case where n is much smaller
    than 2<sup>32</sup> - but the same goes for using the default fraction.
5.  <a name="cite_note-4"></a>[↑](#cite_ref-4)
    [L'Ecuyer, Pierre. 1998. *Good parameters and implementations for
    combined multiple recursive random number
    generators*](http://www.iro.umontreal.ca/~lecuyer/myftp/papers/combmrg2.ps)
6.  <a name="cite_note-5"></a>[↑](#cite_ref-5)
    I have never understood why people insist on (55, 24), especially
    since one often uses an array of 256 "random" numbers anyway, in
    order to take advantage of C and similar languages' implicit modulo
    256 arithmetic on `unsigned char` indices. (255, 52) gives a
    **much** longer period for absolutely no more effort.
7.  <a name="cite_note-6"></a>[↑](#cite_ref-6)
    [It seems](http://en.wikipedia.org/wiki/Alea_iacta_est)
    that when Caesar is supposed to have said "Iacta alea est", he
    actually said "Ἀνερρίφθω κύβος".

