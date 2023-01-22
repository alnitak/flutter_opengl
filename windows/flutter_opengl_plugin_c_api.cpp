#include "include/flutter_opengl/flutter_opengl_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "flutter_opengl_plugin.h"

void FlutterOpenglPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  flutter_opengl::FlutterOpenglPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
