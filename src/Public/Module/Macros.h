#pragma once

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define API_EXPORT __declspec(dllexport)
    #define API_IMPORT __declspec(dllimport)
#else
    #define API_EXPORT __attribute__((visibility("default")))
    #define API_IMPORT __attribute__((visibility("default")))
#endif
