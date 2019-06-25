// Copyright 2019 Marco Bavagnoli <marco.bavagnoli@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package com.bavagnoli.flutteropengl;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.IOException;

import io.flutter.plugin.common.PluginRegistry.Registrar;

/**
 * Used by NDK code to read assets images
 */
public class BmpManager {
    static String TAG = BmpManager.class.getSimpleName();
    static private Registrar mRegistrar;

    /**
     * Inizialized when the app starts in FlutteropenglPlugin.registerWith()
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
