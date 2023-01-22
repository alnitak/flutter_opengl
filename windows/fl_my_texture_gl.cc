#include "fl_my_texture_gl.h"

FlMyTextureGL::FlMyTextureGL(flutter::TextureRegistrar *texture_registrar, 
    int w, int h) 
    : texture_registrar_(texture_registrar)
{
    width = w;
    height = h;
    buffer = std::vector<uint8_t>(width * height * 4 * 4);
    texture_ = std::make_unique<flutter::TextureVariant>(
        flutter::PixelBufferTexture(
            [=](size_t width, size_t height) -> const FlutterDesktopPixelBuffer *
            {
                const std::lock_guard<std::mutex> lock(mutex_);
                return &flutter_pixel_buffer_;
            }));

    texture_id_ = texture_registrar_->RegisterTexture(texture_.get());
}

void FlMyTextureGL::Update()
{
    // const std::lock_guard<std::mutex> lock(mutex_);
    flutter_pixel_buffer_.buffer = buffer.data();
    flutter_pixel_buffer_.width = width;
    flutter_pixel_buffer_.height = height;
    texture_registrar_->MarkTextureFrameAvailable(texture_id_);
}

FlMyTextureGL::~FlMyTextureGL()
{
    texture_registrar_->UnregisterTexture(texture_id_);
    buffer.clear();
    // free(flutter_pixel_buffer_.release_context);
}

void FlMyTextureGL::SetReleaseCallback(void(* release_callback)(void* release_context)) {
    this->flutter_pixel_buffer_.release_callback = release_callback;
}

void FlMyTextureGL::SetReleaseContext(void* release_context) {
    this->flutter_pixel_buffer_.release_context = release_context;
}