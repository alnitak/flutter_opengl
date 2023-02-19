#!/bin/bash

wget https://github.com/opencv/opencv/releases/download/4.7.0/opencv-4.7.0-android-sdk.zip

unzip opencv-4.7.0-android-sdk.zip

rm -frd ../android/src/opencv
cp -r ./OpenCV-android-sdk/sdk/native/libs ../android/src/opencv
cp -r ./OpenCV-android-sdk/sdk/native/jni/include ../android/src/opencv

