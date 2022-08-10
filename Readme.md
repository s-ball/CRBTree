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

A specific case in current version is the `EXPORT` macro. It allows the
automatic generation of an import library on Microsort Visual Studio. On 
a different system, it make probably sense to define `EXPORT` to be an
empty string (what I currently do to build the library on Ubuntu).

### Developper usage

Because of the liberal MIT license, you are allowed to use and modify this
source at will. In order to ease development, a number of test files based
on [Google Test](https://github.com/google/googletest) are provided. The
current coverage is over 95% (according to
[llvm-cov](https://llvm.org/docs/CommandGuide/llvm-cov.html)), so regressions 
or incompatibilitis should be soon detected.

The tests files currently show no memory leak using 
[Valgrind](https://valgrind.org/).

### Public API

The public API is documented on the [API.md](API.md) page.

### The `dump.c` file and the `RBdump` function

`RBdump` is a tool to easily display the content of a tree  with parent-child
dependencies and red/black *color*. It is really dependant to implentation
details and because of that is not member of the public API. Nevertheless, it
can be an useful tool if someone wants to develop over this library.

```
int RBdump ( RBTree* tree, size_t elt_width, void(*dump)(void*, char*))
```

Dumps a tree with parent child relations and node colors.
 
It uses 2 parameters, the width that will be used to display an element
and the function that generates that label. The buffer is internally
managed by RBdump and is passed to the user provided function. Undefined
Behaviour is to be expected if that function writes more than `elt_width`
characters or omits the terminating null...
 
On output, red nodes have their label followed with a `*`.

Parameters

* tree : the tree to dump
* elt_width : the width (in characters) used to display an element
* dump : a user provided function to generate a text label per element

Returns
    : 0 if the tree could successfully be dumped

## Contributions

I will be glad to recieve issues in GitHUB, either for current problems or
possible improvements or additional features. I shall try to answer them, but
as I am not developping (nor even using) this professionaly, I may sometimes
not be very reactive.

## Versioning
This project uses a Major.Minor.Patch[.Sub] versioning pattern. Inside
 a major version, public API stability is expected (at least after 1.0.0
 version will be published).

Unfortunately, the version is only accurate for a Git *tagged* release. On
any other commit, you can only expect to be *close* to what the release
actually is.

Currently, the `.Sub` part is never present Git tagged release, and is
expected to be 0 for those releases. It is intented to increment between
those releases, with no specific pattern, but even that is not fully
reliable.
## License
This project is licensed under the MIT License - see the
[LICENSE.txt](LICENSE.TXT) file for details.

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