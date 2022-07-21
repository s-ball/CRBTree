# CRBtree

A C library for managing Red Black binary trees.

## Current status:

This is a beta quality project because it has never be used in a real world
application. It is developped using Visual
Studio Community 2022, but is intended to be portable to any C platform.
Tests use the *Google Test* framework and require a C++ environment.

Nevertheless, the code coverage level (measured using llvm-cov) is over 95%:
only memory allocation failures are not covered...

## Goals:

There are already a number of implementations of Red Black trees in a
 variety of languages. This one is intended to be provided under a
 liberal license (the well known MIT license) to be useable in any kind of 
 project. As an (interesting for me) side goal, it should be a way to 
 improve my C skills...

## Features:

The library allows to:

* create a new tree handling pointers to void (`void *`), given a
 comparison function
* insert new elements in the tree
* search elements in the tree, returning either a null pointer or a pointer
 to the next existing element when the passed key is not found
* delete elements from the tree
* iterate the tree from the beginning or from a key
* destroy a whole tree in a single operation and optionaly release its
 elements if passed a deleting function

Bulk insertions from an array of elements or from another tree should be
added in a future version.

## Usage

### End user usage:

The library consists of only 2 source files (`rbtree.c` for almost everything
 and `dump.c` for the *dump* feature) and 2 include files, of which only one
 (`rbtree.h`) is to be included in source files willing to use the library.

The recommended usage is then to just add those files to your project and
 include `rbtree.h` in any file using the library.

### Developper usage

Because of the liberal MIT license, you are allowed to use and modify this
source at will. In order to ease development, a number of test files based
on [Google Test](https://github.com/google/googletest) are provided. The
current coverage is over 95% (according to
[llvm-cov](https://llvm.org/docs/CommandGuide/llvm-cov.html)), so regressions 
or incompatibilitis should be soon detected.

The tests files currently show no memory leak using 
[Valgrind](https://valgrind.org/).

## Contributions

I will be glad to recieve issues in GitHUB, either for current problems or
possible improvements or additional features. I shall try to answer them, but
as I am not developping (nor even using) this professionaly, I may sometimes
not be very reactive.

## Versioning
This project uses a standard Major.Minor.Patch versioning pattern. Inside
 a major version, public API stability is expected (at least after 1.0.0
 version will be published).

## License
This project is licensed under the MIT License - see the LICENSE.txt file
 for details

## Acknowledgements:

I should not have been able to develop this without a number of no cost
development tools:

* Microsoft Visual Studio: while it is a proprietary tool, it is a nice IDE
that can be used at no cost by hobbyists
* Clang and more generally the excellent LLVM tools family: they give 
clear messages for most stupid mistakes, and include an excellent coverage
tool
* Valgrind: it allowed to find and fix a number of memory leaks...
* WSL using Ubuntu-20.04: an excellent integration of all the Unix world
development tools on my Windows box, and much more lightweight than a true
virtual machine