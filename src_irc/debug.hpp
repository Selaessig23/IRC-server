#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>

#ifdef DEBUG
#define DEBUG_PRINT(x)                         \
  do {                                         \
    std::cerr << "[DEBUG IRC] " << x << std::endl; \
  } while (0)
#else
#define DEBUG_PRINT(x) \
  do {                 \
  } while (0)
#endif

#endif  // DEBUG_HPP
