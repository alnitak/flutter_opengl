#ifndef FLUTTER_PLUGIN_FLUTTER_OPENGL_PLUGIN_H_
#define FLUTTER_PLUGIN_FLUTTER_OPENGL_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace flutter_opengl {

class FlutterOpenglPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  FlutterOpenglPlugin();

  virtual ~FlutterOpenglPlugin();

  // Disallow copy and assign.
  FlutterOpenglPlugin(const FlutterOpenglPlugin&) = delete;
  FlutterOpenglPlugin& operator=(const FlutterOpenglPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace flutter_opengl

#endif  // FLUTTER_PLUGIN_FLUTTER_OPENGL_PLUGIN_H_
