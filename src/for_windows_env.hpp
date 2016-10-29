#ifndef FOR_WINDOWS_ENV_HPP
#define FOR_WINDOWS_ENV_HPP

#if defined(_WIN32)
  #ifdef min
    #undef min
  #endif
  #ifdef max
    #undef max
  #endif
#endif

#endif // FOR_WINDOWS_ENV_HPP
