#include "include/flutter_opengl/flutter_opengl_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <flutter_linux/fl_view.h>
#include <sys/utsname.h>
#include <glib.h>

#include <GL/glew.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <future>
#include <chrono>

#include "include/fl_my_texture_gl.h"
#include "../src/ffi.h"
#include "../src/common.h"
#include "include/test.h"

G_DEFINE_TYPE(FlutterOpenglPlugin, flutter_opengl_plugin, g_object_get_type())

#define EGL_EGLEXT_PROTOTYPES
// https://developer.nvidia.com/blog/egl-eye-opengl-visualization-without-x-server/
// https://developer.nvidia.com/blog/linking-opengl-server-side-rendering/
// void initEGL()
// {

//   glewExperimental = GL_TRUE;
//   GLenum err = glewInit();
//   if (GLEW_OK != err)
//   {
//     /* Problem: glewInit failed, something is seriously wrong. */
//     std::cout << "Error: " << glewGetErrorString(err) << std::endl;
//     return;
//   }

//   static const int MAX_DEVICES = 32;
//   EGLDeviceEXT eglDevs[MAX_DEVICES];
//   EGLint numDevices;

//   PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT =
//       (PFNEGLQUERYDEVICESEXTPROC)
//           eglGetProcAddress("eglQueryDevicesEXT");

//   eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);

//   printf("Detected %d devices\n", numDevices);

//   PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
//       (PFNEGLGETPLATFORMDISPLAYEXTPROC)
//           eglGetProcAddress("eglGetPlatformDisplayEXT");

//   static const EGLint configAttribs[] = {
//       EGL_SURFACE_TYPE,
//       EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
//       EGL_RENDERABLE_TYPE,
//       EGL_OPENGL_ES2_BIT,
//       EGL_RED_SIZE,
//       8,
//       EGL_GREEN_SIZE,
//       8,
//       EGL_BLUE_SIZE,
//       8,
//       EGL_ALPHA_SIZE,
//       8,
//       EGL_NONE,
//   };
//   EGLBoolean found = EGL_FALSE;
//   EGLConfig config;
//   int numconfig;
//   EGLDisplay eglDpy;
//   // https://stackoverflow.com/questions/66096474/how-do-i-set-the-egl-default-display
//   for (EGLint i = 0; (EGL_FALSE == found) && (i < numDevices); ++i)
//   {
//     eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[i], 0);

//     found = eglChooseConfig(eglDpy, configAttribs, &config, 1, &numconfig);
//     printf("**************************** %d  found:%d\n", i, found);
//     eglTest(eglDpy);
//   }

//   eglSwapInterval(eglDpy, 1);
//   eglPrintError("eglSwapInterval");

//   static const int pbufferWidth = 9;
//   static const int pbufferHeight = 9;

//   static const EGLint pbufferAttribs[] = {
//       EGL_WIDTH,
//       pbufferWidth,
//       EGL_HEIGHT,
//       pbufferHeight,
//       EGL_NONE,
//   };

//   // 1. Initialize EGL
//   // /*EGLDisplay*/ eglDpy = eglGetDisplay(x_dpy);
//   // EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
//   eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[1], 0);

//   EGLint major, minor;

//   eglInitialize(eglDpy, NULL, NULL);
//   eglPrintError("eglInitialize()");

//   // 2. Select an appropriate configuration
//   EGLint numConfigs;
//   EGLConfig eglCfg;

//   eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
//   eglPrintError("eglChooseConfig()");

//   // 3. Create a surface
//   EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg,
//                                                pbufferAttribs);
//   eglPrintError("eglCreatePbufferSurface()");

//   // 4. Bind the API
//   eglBindAPI(EGL_OPENGL_ES_API);
//   eglPrintError("eglBindAPI()");

//   // 5. Create a context and make it current
//   // EGLint ctxattr[] = {
//   //   EGL_CONTEXT_CLIENT_VERSION, 2,
//   //   EGL_NONE
//   // };
//   EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,
//                                        NULL);
//   eglPrintError("eglCreateContext()");

//   // eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
//   // eglPrintError("eglMakeCurrent()");

//   // from now on use your OpenGL context

//   // 6. Terminate EGL when finished
//   // eglTerminate(eglDpy);

//   eglTest(eglDpy);
// }



// Called when a method call is received from Flutter.
static void flutter_opengl_plugin_handle_method_call(
    FlutterOpenglPlugin *self,
    FlMethodCall *method_call)
{
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar *method = fl_method_call_get_name(method_call);
  // Get Dart arguments
  FlValue *args = fl_method_call_get_args(method_call);


    /********************************************/
    /*** CREATE SURFACE *************************/
    /********************************************/
    if (strcmp(method, "createSurface") == 0)
    {
      int width = 0;
      int height = 0;
      FlValue *w = fl_value_lookup_string(args, "width");
      FlValue *h = fl_value_lookup_string(args, "height");
      if (w != nullptr)
        width = fl_value_get_int(w);
      if (h != nullptr)
        height = fl_value_get_int(h);
      if (width == 0 || height == 0)
      {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            "100",
            "MethodCall createSurface() called without passing width and height parameters!",
            nullptr));
      }
      else
      {
        if (self->context != nullptr &&
            self->myTexture->width == width &&
            self->myTexture->height == height)
        {
          // g_autoptr(FlValue) result = fl_value_new_int(-1);
          // response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
          fl_texture_registrar_unregister_texture(self->texture_registrar,
                                                  self->texture);
          if (getRenderer() != nullptr)
            stopThread();
        }

        GdkWindow *window = nullptr;
        window = gtk_widget_get_parent_window(GTK_WIDGET(self->fl_view));
        GError *error = NULL;
        self->context = gdk_window_create_gl_context(window, &error);

        gdk_gl_context_make_current(self->context);

        glGenTextures(1, &self->texture_name);
        glBindTexture(GL_TEXTURE_2D, self->texture_name);

        self->myTexture = fl_my_texture_gl_new(GL_TEXTURE_2D, self->texture_name, width, height);
        self->texture = FL_TEXTURE(self->myTexture);
        fl_texture_registrar_register_texture(self->texture_registrar, self->texture);
        fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
                                                          self->texture);
        gdk_gl_context_clear_current();

        ctx_f.context = self->context;
        ctx_f.texture_name = self->texture_name;
        ctx_f.texture_registrar = self->texture_registrar;
        ctx_f.myTexture = self->myTexture;
        ctx_f.texture = self->texture;
        ctx_f.width = width;
        ctx_f.height = height;
        createRenderer(&ctx_f);

        g_autoptr(FlValue) result =
            fl_value_new_int(reinterpret_cast<int64_t>(self->texture));
        response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      }

    } // end createSurface
    else
    {
      response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
    }

  fl_method_call_respond(method_call, response, nullptr);
}

static void flutter_opengl_plugin_dispose(GObject *object)
{
  G_OBJECT_CLASS(flutter_opengl_plugin_parent_class)->dispose(object);
}

static void flutter_opengl_plugin_class_init(FlutterOpenglPluginClass *klass)
{
  G_OBJECT_CLASS(klass)->dispose = flutter_opengl_plugin_dispose;
}

static void flutter_opengl_plugin_init(FlutterOpenglPlugin *self) {}

static void method_call_cb(FlMethodChannel *channel, FlMethodCall *method_call,
                           gpointer user_data)
{
  FlutterOpenglPlugin *plugin = FLUTTER_OPENGL_PLUGIN(user_data);
  flutter_opengl_plugin_handle_method_call(plugin, method_call);
}

void flutter_opengl_plugin_register_with_registrar(FlPluginRegistrar *registrar)
{
  FlutterOpenglPlugin *plugin = FLUTTER_OPENGL_PLUGIN(
      g_object_new(flutter_opengl_plugin_get_type(), nullptr));

  FlView *fl_view = fl_plugin_registrar_get_view(registrar);
  plugin->fl_view = fl_view;
  plugin->texture_registrar =
      fl_plugin_registrar_get_texture_registrar(registrar);

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "flutter_opengl_plugin",
                            FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  // Initialize GL
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    std::cout << "Error: " << glewGetErrorString(err) << std::endl;
    return;
  }
  // initEGL();

  g_object_unref(plugin);
}

// https://gitlab.freedesktop.org/libinput/libinput/-/blob/main/tools/libinput-debug-gui.c

// https://discourse.gnome.org/t/gtk-opengl-widget-drawing-result-broken-when-maximize-restore-window/4165
// https://github.com/flutter/engine/blob/master/shell/platform/linux/fl_gl_area.h
