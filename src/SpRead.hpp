#ifndef SPREAD_HPP
#define SPREAD_HPP

#include <vector>
#include <string>

// We attach this macro to functions that expect thread-safe.
#define THREAD_SAFE_FUNC

typedef std::vector<std::string> StringVec;
typedef std::vector<unsigned char> RawData;

#endif // SPREAD_HPP
