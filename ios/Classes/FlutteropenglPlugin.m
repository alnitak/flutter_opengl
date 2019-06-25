#import "FlutteropenglPlugin.h"
#import <flutteropengl/flutteropengl-Swift.h>

@implementation FlutteropenglPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftFlutteropenglPlugin registerWithRegistrar:registrar];
}
@end
