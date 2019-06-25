## 0.0.4
* I had to change the plugin name because the underscore in the name is causing problems in Android NDK.
  Since I had previously uploaded the package with the old name, I must update it with that old name.
  So "name: flutteropengl" in pubspec.yaml must be uncommented

## 0.0.3

* code rearranged.
* every shader can now be written separately subclassing the Shader class and passing it to the renderer.
* added function to load texture image from assets folder with Android. Since this uses java, it should be rewritten for iOS.
* added a texture shader example.