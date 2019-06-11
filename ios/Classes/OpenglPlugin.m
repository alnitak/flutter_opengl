#import "OpenglPlugin.h"
#import <opengl/opengl-Swift.h>

@implementation OpenglPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftOpenglPlugin registerWithRegistrar:registrar];
}
@end
