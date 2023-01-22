#ifndef FLUTTER_MY_TEXTURE_H
#define FLUTTER_MY_TEXTURE_H

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <mutex>

class FlMyTextureGL
{
public:
    FlMyTextureGL(flutter::TextureRegistrar *texture_registrar,
            int w, int h);

    int64_t texture_id() const { return texture_id_; }

    void Update();

    ~FlMyTextureGL();

    void SetReleaseCallback(void (* release_callback)(void* release_context));
    void SetReleaseContext(void *release_context);

    FlutterDesktopPixelBuffer flutter_pixel_buffer_{};

    std::vector<uint8_t> buffer;
    int32_t width;
    int32_t height;
private:
    flutter::TextureRegistrar *texture_registrar_ = nullptr;
    std::unique_ptr<flutter::TextureVariant> texture_ = nullptr;
    int64_t texture_id_;
    mutable std::mutex mutex_;  
};

typedef struct {
    flutter::MethodChannel<flutter::EncodableValue>* channel;
    uint8_t* buffer;
} user_release_context;

#endif // FLUTTER_MY_TEXTURE_H