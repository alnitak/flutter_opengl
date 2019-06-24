package com.bavagnoli.opengl;

import android.app.Activity;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;

import io.flutter.plugin.common.PluginRegistry.Registrar;

/**
 * Used by NDK code to read assets images
 */
public class BmpManager {
    static String TAG = BmpManager.class.getSimpleName();
    static private Registrar mRegistrar;

    /**
     * Inizialized when the app starts in OpenglPlugin.registerWith()
     * @param activity
     */
    public static void setAssetsManager(Registrar registrar) {
        mRegistrar = registrar;
    }

    // https://flutter.dev/docs/development/ui/assets-and-images
    private AssetFileDescriptor getAssetsImage(String imagePath) {
        Log.i(TAG, "getAssetsImage: " + imagePath);
        AssetFileDescriptor fd = null;
        try {
            AssetManager assetManager = mRegistrar.context().getAssets();
            String key = mRegistrar.lookupKeyForAsset(imagePath);
            fd = assetManager.openFd(key);
        } catch (IOException e) {
            Log.e(TAG, "getAssetsImage error: " + e );
        }
        return fd;
    }

    public Bitmap open(String path)
    {
        AssetFileDescriptor fd = getAssetsImage(path);
        Log.i(TAG, "************************ open: " + path + "   " + fd.getFileDescriptor().toString());
        try
        {
            return BitmapFactory.decodeStream(fd.createInputStream());
        }
        catch (Exception e) {
            Log.e(TAG, "open error: " +e + " cannot open: " + path );
        }
        return null;
    }

    public int getWidth(Bitmap bmp) { return bmp.getWidth(); }
    public int getHeight(Bitmap bmp) { return bmp.getHeight(); }

    public void getPixels(Bitmap bmp, int[] pixels)
    {
        int w = bmp.getWidth();
        int h = bmp.getHeight();
//        bmp = bmp.copy(Bitmap.Config.ARGB_8888, false);
        bmp.getPixels(pixels, 0, w, 0, 0, w, h);
    }

    public void close(Bitmap bmp)
    {
        bmp.recycle();
    }
}
