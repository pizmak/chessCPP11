#pragma once

#ifdef DEBUG

#define TRACE(message) std::cerr << message
#define TRACELN(message) std::cerr << message << std::endl

#define _TRACE_PARAM(p) #p << " = " << p
#define _LEFT_SHIFT() << ", " <<
#define TRACE_PARAMS(...) \
    std::cerr << __FUNCTION__ << "(";\
    std::cerr << FOREACH(_TRACE_PARAM, _LEFT_SHIFT, __VA_ARGS__);\
    std::cerr << ")" << std::endl;

#else

#define TRACE(message)
#define TRACELN(message)
#define TRACE_PARAMS(...)

#endif

