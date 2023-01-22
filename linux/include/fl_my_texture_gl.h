#ifndef FLUTTER_MY_TEXTURE_H
#define FLUTTER_MY_TEXTURE_H

#include <gtk/gtk.h>
#include <glib-object.h>
#include "flutter_opengl/flutter_opengl_plugin.h"
#include <flutter_linux/flutter_linux.h>


G_DECLARE_FINAL_TYPE(FlMyTextureGL,
                     fl_my_texture_gl,
                     FL,
                     MY_TEXTURE_GL,
                     FlTextureGL)

struct _FlMyTextureGL
{
    FlTextureGL parent_instance;
    uint32_t target;
    uint32_t name;
    uint32_t width;
    uint32_t height;
};


#define FLUTTER_OPENGL_PLUGIN(obj)                                     \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), flutter_opengl_plugin_get_type(), \
                              FlutterOpenglPlugin))

struct _FlutterOpenglPlugin
{
  GObject parent_instance;
  GdkGLContext *context = nullptr;
  FlTextureRegistrar *texture_registrar = nullptr;
  FlMyTextureGL *myTexture = nullptr;
  unsigned int texture_name = 0;
  g_autoptr(FlTexture) texture;
  FlView *fl_view = nullptr;
};


FlMyTextureGL *fl_my_texture_gl_new(uint32_t target,
                                    uint32_t name,
                                    uint32_t width,
                                    uint32_t height);
#endif // FLUTTER_MY_TEXTURE_H
