package com.example.flutter_opengl;

import android.graphics.SurfaceTexture;
import android.util.Log;
import android.view.Surface;

import androidx.annotation.NonNull;

import java.util.Map;

import io.flutter.embedding.engine.plugins.FlutterPlugin;
import io.flutter.plugin.common.MethodCall;
import io.flutter.plugin.common.MethodChannel;
import io.flutter.plugin.common.MethodChannel.MethodCallHandler;
import io.flutter.plugin.common.MethodChannel.Result;
import io.flutter.view.TextureRegistry;

/**
 * FlutterOpenglPlugin
 */
public class FlutterOpenglPlugin implements FlutterPlugin, MethodCallHandler {
    private static String TAG = FlutterOpenglPlugin.class.getSimpleName();
    static {
        System.loadLibrary("flutter_opengl_plugin");
    }

    public static native void nativeSetSurface(Surface surface, int width, int height);

    /// The MethodChannel that will the communication between Flutter and native Android
    ///
    /// This local reference serves to register the plugin with the Flutter Engine and unregister it
    /// when the Flutter Engine is detached from the Activity
    private MethodChannel channel;

    private TextureRegistry textures;
    TextureRegistry.SurfaceTextureEntry entry;
    SurfaceTexture surfaceTexture;
    int width, height;

    private double getDouble(Object obj) {
        if (obj instanceof Double)
            return (Double) obj;
        return -1;
    }

    private int getInt(Object obj) {
        if (obj instanceof Integer)
            return (int) obj;
        return -1;
    }

    private int getBoolean(Object obj) {
        if (obj instanceof Boolean)
            return ((Boolean) obj).booleanValue() ? 1 : 0;
        return -1;
    }

    private String getString(Object obj) {
        if (obj instanceof String)
            return ((String) obj).toString();
        return "";
    }

    @Override
    public void onAttachedToEngine(@NonNull FlutterPluginBinding flutterPluginBinding) {
        textures = flutterPluginBinding.getTextureRegistry();
        channel = new MethodChannel(flutterPluginBinding.getBinaryMessenger(), "flutter_opengl_plugin");
        channel.setMethodCallHandler(this);
    }

    @Override
    public void onMethodCall(@NonNull MethodCall call, @NonNull Result result) {
        final Map<String, Object> arguments = (Map<String, Object>) call.arguments;
        if (arguments != null)
            Log.d("FlutteropenglPlugin", call.method + " " + call.arguments.toString());

        if (call.method.equals("draw")) {
            // for testing purpose
        } else
        if (call.method.equals("createSurface")) {
            width = (int)getInt(arguments.get("width"));
            height = (int)getInt(arguments.get("height"));

            // check if all arguments are sane
            if (width == -1) {
                Log.i(TAG, "onMethodCall: width not passed correctly!");
                result.error("onMethodCall()", "onMethodCall: width not passed correctly!", null);
                return;
            }
            if (height == -1) {
                Log.i(TAG, "onMethodCall: height not passed correctly!");
                result.error("onMethodCall()", "onMethodCall: height not passed correctly!", null);
                return;
            }


            entry = textures.createSurfaceTexture();
            surfaceTexture = entry.surfaceTexture();
            Surface surface = new Surface(surfaceTexture);
            nativeSetSurface(surface, width, height);
            result.success(entry.id());
        } else {
            result.notImplemented();
        }
    }

    @Override
    public void onDetachedFromEngine(@NonNull FlutterPluginBinding binding) {
        channel.setMethodCallHandler(null);
    }
}
