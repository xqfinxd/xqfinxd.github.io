#pragma once
#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#include "Texture.h"
class DLL_API ImageHelper {
public:
    static Texture Load(const char* file);
};

