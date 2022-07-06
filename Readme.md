# CRBtree

A C library for managing Red Black binary trees.

## Current status:

This is currently only a work in progress. It is developped using Visual
Studio Community 2022, but is intended to be portable to any C platform.
Tests use the *Google Test* framework and require a C++ environment.

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

[ to be done]

## Contributions

As soon as the first  release version will be published, contributions will
 be welcome. Currently, it is really too much of a *work in progress*...

## Versioning
This project uses a standard Major.Minor.Patch versioning pattern. Inside
 a major version, public API stability is expected (at least after 1.0.0
 version will be published).

## License
This project is licensed under the MIT License - see the LICENSE.txt file
 for details