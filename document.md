

# Notes and Documentation

This document is intended to record pertinent details, discoveries, or
investigations of this code.


This is very much work-in-progress.

I might even want to break this into sub-documents.


# `reduce.c`

## Data structures

This program makes use of several data structures, but those data structures
take the form of a large two-dimensional array of `long` values. This is
unfortunate for two reasons: the data structure is opaque without external
explanation, and the fact that the size of `long` is platform-dependent.

### `tp_confmat`

See section 1, "Configurations", of `RSST/reduce.pdf`.

This data structure represents a configuration.

Assume the declaration `tp_confmat A;` has been made.

#### The header

The first four values of the configuration, `A[0][0..3]` have the following
meanings:

1. `A[0][0]`: This is the number of vertices in the configuration.
2. `A[0][1]`: This is the number of vertices in the outer ring.
3. `A[0][2]`: The number of canonical colourings in \cal{C}(K). Apparently, this
   is the number of colourings that can be extended to the configuration.
4. `A[0][3]`: The number of canonical colourings in \cal{C}'(K). Apparently,
   this is the number of colourings in the maximal consistent subset.

#### The contract

The values `A[0][4..13]` (inclusive) specify the "contract" of the
configuration. This is related to C- versus D- reducibility, I believe.
D-Reducible configurations do not need a contract.

`A[0][4]` is the number of edges in the contract. The remaining values are
between 0 and 4 edges, specified as pairs of vertices.

#### Vertices

The remaining rows of the matrix (`A[i]` for `i > 0`) are the vertices of the
configuration.

### `tp_edgeno`

### `tp_angle`

However, there are several other data structures that were not given names.

### The live set


### Contracts

A contract has two meanings in the program:

1. for `tp_confmat A`, the values `A[0][4..13]`.
2. An array of long values. (used as paramter to `findangles`, etc.).

This section discusses the latter definition.

### `simatchnumber`

An array of large numbers. The name is probably derived from "[number] of
[si]gned [match]ings".

TODO: RSST vs. JPS definitions of `simatchnumber`.

## That one place with the `char`

The function `checkreality()`.

There is one place in the program that uses a `char` to select bits from an
array of `long`s. Unfortunately, the method by which this is done is to
repeatedly left-shift the char, and if the char overflows wrapping to zero, to
increment the index to the next byte.

Doubly annoying is the fact that this char is hidden behind a pointer that is
threaded through a lot of stuff, especially augment.

## How the program works

I'm still working on this part.

## Parsing Configurations and the Configuration file Format

TODO: Write about the format, and the well-formedness conditions.



# `discharge.c`

## How to Invoke

Most relevantly, the input to `discharge` is a *presentation file*, along with
an unavoidable set and a set of *rules*.

for example, to test Robertson et al's proof script of degree 7, the following
command would be invoked.

```bash
$ discharge robertson/present7 robertson/unavoidble.conf robertson/rules 0 1
#			^				   ^						 ^				 ^ ^
#			|				   |						 |				 | |
#			The presentation   The unavoidable set		 The rules file  | printing mode
#			file														 line number
```

The final two parameters need explanation, which can be found by looking at the
definition of `main()`, in particular the parsing of the command line arguments.

### Printing mode

This option controls the verbosity. It takes values 1, 2, 3, or 4.

1. Print only the input line.
2. Print "basic" information.
3. Print hubcaps but not reducibility.
4. Print hubcaps and reducibility.

### Line number

This option controls what lines have information about them printed.

If this option is `0`, information is printed about all lines (as above).

If positive, only that line has its information printed.

If negative, information about no line is printed. However, I suspect that this
behavior is accidental, and thus should not be relied upon.


## How it works

I have negligible idea.

Each line of the presentation file is like a command or an operation:

TODO: Enumerate operations (see the `switch` statement in `main()`)
TODO: Describe each operation and how it works


# References

Mostly, these notes come the code itself and the documents bundled with this
code, in the directory `RSST/`.

* `reduce.pdf`: Reducibility in the Four-Color Theorem
* `discharge.pdf`: Discharging Cartwheels

Additionally, it may be helpful to look at other papers on proofs of the
Four-Color Theorem, such as:

* Robertson et al: The Four Color Theorem (1997)
* Steinberger: An Unavoidable Set of D-Reducible Configurations (2009)

<!-- vim: set tw=80: -->
