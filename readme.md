# mincore
C++20 **Min**imal **Core** Library.

Currently supports Windows and Linux.

Can only build with clang, since the library heavily uses clang intrinsics for atomics and type traits.

## Goals

The goal of the library is to provide an alternative (and not a replacement) of the C++ Standard Library. That means not searching for compliancy with the standard.

In addition to providing identical implementations on multiple platforms, this allows to have simpler, lightweight structures without all the bloat that implementations can have (Iterator debug levels, unordered map locked to a linked list implementation, ...)

Some examples of not compliancy with the standard:

- array_view is the std's span equivalent, but with an implementation much closer to the string_view.
- atomics supports only lock-free operations.
- string is not templated with the char type, and supports only `char` (a `wchar_t` version may be created if needed).
- vector only has `emplace_back` to insert elements at the end
- (contiguous) containers manipulations don't take iterators, only index (iterators may be implemented later to provide container agnostic algorithms)
- ...

One explicit non-goal of this library is to add wrappers for OS dependant primitives, e.g. threads, locks, I/O, ... The decision to this is because every OSes don't have the same concepts for these primitives, and thus making them hard to abstract without potential performance drawbacks.

The only exception referencing OS is for the placement new definition, which is more Standard Library specific than OS specific.

## Features

***Note***: The library is currently more a hobby project, and is missing many features to make it useable in large, complexes codebase. The library will grow over time, depending on the specific needs I will have for other projects using it.

### Containers

Containers currently don't support allocators, and can only refer to one global allocator present in [alloc.hh](/src/alloc.hh). Support for them may be added later.

```cpp
array<T, Size> // stack array, wrapper for a C array with some helpers functions.
array_view<T>  // Array view, not owning the data. Can work for C arrays, array<T> as well as vector<T>.
vector<T>      // Dynamic array. Roughly equivalent to std::vector<T>.

string         // Dynamic string. Supports only `char` type.
string_view    // String view, not owning the data. Can work for string literals as well as string<T>.
```

### Others

#### [`atomic.hh`](/src/atomic.hh)
Atomics wrapper. Works only with lock-free data.

#### [`compare.hh`](/src/compare.hh)
std's replacement for three-way comparison operators (`<=>`). std namespace is needed due to implementation specificities with compilers. Can't work side by side with official std's implementation (see [config.hh](/src/config.hh) for details).

#### [`concepts.hh`](/src/concepts.hh)
General concepts definitions. Since C++20 allow concept uses for `if constexpr`, all constant bool type traits have been moved as concepts, to allow a broader use and split better different usages.

#### [`enum.hh`](/src/enum.hh)
Enum helpers, mainly used for `enum class`.

#### [`initializer_list.hh`](/src/initializer_list.hh)
std's replacement for initializer lists. std namespace is needed due to implementation specificities with compilers. Can't work side by side with official std's implementation (see [config.hh](/src/config.hh) for details).

#### [`pair.hh`](/src/pair.hh)
Implementation of templated pair structure. Roughly equivalent to std's pair.

#### [`type_traits.hh`](/src/type_traits.hh)
General type traits helpers. Only defines type traits, and not bool constant, as they are implemented in [`concepts.hh`](/src/concepts.hh)

#### [`utility.hh`](/src/utility.hh)
General helpers structs/functions. This file may disappear to create multiple, more specific files as it will cause issues in the end with inclusion time.

## Building

### For use in your project
Simply drop the sources in your project root, and compile them with C++20 enabled. Use [`config.hh`](/src/config.hh) to customise the library.

Alternatively, use [mingen](https://github.com/BluTree/mingen) and use the project defined in mingen.lua as a dependency of yours.

### Unit Tests
Uses [mingen](https://github.com/BluTree/mingen) for tests compilation. Generates the project with it, and compile it.

A custom configuration is provided to enable code coverage reports using clang features, named `debug_coverage`.

Tested on:
- Windows: clang 18+
- Linux: clang 18+