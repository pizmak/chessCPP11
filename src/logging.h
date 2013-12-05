#pragma once

#ifdef DEBUG

#define TRACE(message) std::cerr << message
#define TRACELN(message) std::cerr << message << std::endl

#else

#define TRACE(message)
#define TRACELN(message)

#endif
