#include "flutter_opengl_plugin.h"
#include "../src/common.h"
#include "../src/Shader.h"
#include "fl_my_texture_gl.h"
#include "../src/ffi.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>


#include <random>
#include <GL/glew.h>
// #include <EGL/egl.h>
// #include <GLES3/gl3.h>

namespace flutter_opengl {

GLint MySetPixelFormat()
  {

    // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-choosepixelformat
   static PIXELFORMATDESCRIPTOR pfd=    
    { 
    sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
    1,                     // version number  
    PFD_DRAW_TO_WINDOW |   // support window  
    PFD_SUPPORT_OPENGL |   // support OpenGL  
    PFD_DOUBLEBUFFER,      // double buffered  
    PFD_TYPE_RGBA,         // RGBA type  
    24,                    // 24-bit color depth  
    0, 0, 0, 0, 0, 0,      // color bits ignored  
    0,                     // no alpha buffer  
    0,                     // shift bit ignored  
    0,                     // no accumulation buffer  
    0, 0, 0, 0,            // accum bits ignored  
    32,                    // 32-bit z-buffer      
    0,                     // no stencil buffer  
    0,                     // no auxiliary buffer  
    PFD_MAIN_PLANE,        // main layer  
    0,                     // reserved  
    0, 0, 0                // layer masks ignored  
    }; 

   GLint  iPixelFormat; 

   // get the device context's best, available pixel format match 
   if((iPixelFormat = ChoosePixelFormat(ctx_f.hdc, &pfd)) == 0)
   {
    std::cout << "ChoosePixelFormat Failed" << std::endl;
    return 0;
   }


   // make that match the device context's current pixel format 
   if(SetPixelFormat(ctx_f.hdc, iPixelFormat, &pfd) == FALSE)
   {
    std::cout << "SetPixelFormat Failed" << std::endl;
    return 0;
   }

   if((ctx_f.hrc = wglCreateContext(ctx_f.hdc)) == NULL)
   {
    std::cout << "wglCreateContext Failed" << std::endl;
    return 0;
   }

   if((wglMakeCurrent(ctx_f.hdc, ctx_f.hrc)) == NULL)
   {
    std::cout << "wglMakeCurrent Failed" << std::endl;
    return 0;
   }


   return 1;
  }

void initGL() 
{

    ctx_f. m_hWnd = ctx_f.registrar->GetView()->GetNativeWindow();       // rendering context variable  
    /* Get the handle of the windows device context. */ 
    ctx_f.hdc = GetDC(ctx_f.m_hWnd); 
 
    wglMakeCurrent(ctx_f.hdc, NULL);
    MySetPixelFormat();

    // Initialize GL
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      std::cout << "Error: " << glewGetErrorString(err) << std::endl;
      return;
    }
}

void FlutterOpenglPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "flutter_opengl_plugin",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<FlutterOpenglPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });



  ctx_f.registrar = registrar;
  ctx_f.texture_registrar = registrar->texture_registrar();
  

  registrar->AddPlugin(std::move(plugin));
}

FlutterOpenglPlugin::FlutterOpenglPlugin() {}

FlutterOpenglPlugin::~FlutterOpenglPlugin() {
    deleteRenderer();
}

std::vector<uint8_t> buffer;
void FlutterOpenglPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("draw") == 0) {
    
    buffer.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 255);
    for (int i = 0; i < ctx_f.height * ctx_f.width * 4; ++i) {
        buffer.push_back(
            distr(gen)
            );
    }
    bool b = wglMakeCurrent(ctx_f.hdc, ctx_f.hrc);
    eglPrintError("glGenTextures");
    glBindTexture(GL_TEXTURE_2D, ctx_f.texture_name); 
    eglPrintError("glBindTexture");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ctx_f.width, ctx_f.height, 
            0, GL_BGR, GL_UNSIGNED_BYTE, buffer.data());
    eglPrintError("glTexImage2D");
SwapBuffers( ctx_f.hdc );

    // ctx_f.myTexture->Update(buffer.data(), ctx_f.height, ctx_f.width);

    ctx_f.texture_registrar->MarkTextureFrameAvailable(ctx_f.texture_id);
    result->Success( flutter::EncodableValue(true) );

  } else
  if (method_call.method_name().compare("createSurface") == 0) {
    flutter::EncodableMap arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
    
    
    // initGL();
    // bool b = wglMakeCurrent(ctx_f.hdc, ctx_f.hrc);
    // glGenTextures(1, &ctx_f.texture_name);
    // eglPrintError("glGenTextures");
    // glBindTexture(GL_TEXTURE_2D, ctx_f.texture_name);
    createRenderer(&ctx_f);


    ctx_f.width = std::get<int>(arguments[flutter::EncodableValue("width")]);
    ctx_f.height = std::get<int>(arguments[flutter::EncodableValue("height")]);

    ctx_f.m_hWnd = ctx_f.registrar->GetView()->GetNativeWindow();
    if (ctx_f.myTexture != nullptr)
    {
      delete ctx_f.myTexture;
      ctx_f.myTexture = nullptr;
    }
    ctx_f.myTexture = new FlMyTextureGL(ctx_f.texture_registrar, ctx_f.width, ctx_f.height);
    ctx_f.texture_id = ctx_f.myTexture->texture_id();


    
    result->Success( flutter::EncodableValue(ctx_f.texture_id) );
  } else {
    result->NotImplemented();
  } 
}

}  // namespace flutter_opengl
