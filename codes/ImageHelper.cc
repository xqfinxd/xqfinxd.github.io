#include "ImageHelper.h"
#include "third-party/stb_image.h"

Texture ImageHelper::Load(const char* file) {
    int x, y, c;
    auto deleter = [](uint8_t* data) {
        stbi_image_free(data);
    };
    std::unique_ptr<uint8_t, void(*)(uint8_t*)> data =
        std::unique_ptr<uint8_t, void(*)(uint8_t*)>(stbi_load(file, &x, &y, &c, 0), deleter);
    if (!data) {
        return Texture();
    }
    return Texture(data.get(), x, y, c);
}