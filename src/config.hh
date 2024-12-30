#pragma once

// Use default, std implementation of comparison order classes. This is needed for
// operator <=>.
// If you are including std headers alongside mincore, you should enable this or
// compilation errors will occur (class redefinition)
// #define COMPARE_USE_STD

// Use default, std implementation of aggregate list. This is needed for list constructor
// with {}.
// If you are including std headers alongside mincore, you should enable this or
// compilation errors will occur (class redefinition)
// #define INITIALIZER_LIST_USE_STD

// Use default, std implementation placement new function.
// If you are including std headers alongside mincore, you should enable this or
// compilation errors will occur (function redefinition)
// #define NEW_USE_STD