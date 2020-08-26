#pragma once
#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#include <memory>
class DLL_API Texture {
public:
    Texture() : width(0), height(0), channel(0), data(nullptr) {}
    Texture(const Texture& old) : width(old.width), height(old.height), channel(old.channel), data(old.data) {
    }
    Texture(const uint8_t* data, int width, int height, int channel) :
        width(width), height(height), channel(channel) {
        this->data = std::shared_ptr<uint8_t[]>(new uint8_t[width * height * channel]);
        memcpy(this->data.get(), data, sizeof(uint8_t) * width * height * channel);
    }
    Texture& operator = (const Texture& old) {
        width = old.width;
        height = old.height;
        channel = old.channel;
        data = old.data;
        return *this;
    }
    operator bool() {
        if (!data || width == 0 || height == 0 || channel == 0) {
            return false;
        } else {
            return true;
        }
    }
    ~Texture() {
        width = 0;
        height = 0;
        channel = 0;
    }
    uint32_t width, height, channel;
    std::shared_ptr<uint8_t[]> data = nullptr;
};
