# minitrig

## A small library of efficient and accurate trigonometric functions.

Minitrig is a library which is intended as a reference for implementing one-off trigonometric functions.  This is useful, for example, in cases where `<cmath>`/`<math.h>` doesn't exist or is incomplete, such as in certain embedded systems (minitrig was invented for just such a purpose).  Note, however, that minitrig is based on floating-point, not integer arithmetic.  Another usage is as a platform for testing new trigonometric algorithms.

Usage: build files are provided for MSVC 2015.  Simply include `libminitrig/include-f32.hpp` and use the functions in `minitrig::`.

Minitrig is free (as in both freedom and beer).  Your contributions of reasonably simple code, including alternate implementations of existing functions and new functions, are welcome.