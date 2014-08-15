NDKSignalTest
=============

- Shows a crash on Android 4.3+ devices (Nexus 7 devices tested).

- This may be interesting to Android developers working on libc (bionic) and/or Linux kernel

Setup
-----

- Edit local.properties to specify path to Android NDK/SDK so you can build this

- Determine if you want to run the super-reduced testcase or create a minimal Android app.  For the
  latter you will need to edit the `jni/Android.mk` file to comment the `include
  $(BUILD_EXECUTABLE)` line and uncomment the `include $(BUILD_SHARED_LIBRARY)` line


To build a super-reduced ADB commandline testcase
-------------------------------------------------

- Change into the jni directory and execute `ndk-build`

- Copy the resulting exe using ADB to an executable directory device, e.g. for a Nexus 7 running
  Android 4.2+ : `adb push ../libs/armeabi/NDKSignalTest /data/local/tmp/`

- Possibly you may need to add executable permissions : `adb shell chmod 755 /data/local/tmp/NDKSignalTest`

- Execute over adb : `adb shell /data/local/tmp/NDKSignalTest`

Build a reduced testcase as a standard Android app
--------------------------------------------------

- Swizzle the commented lines in `jni/Android.mk`

- With a USB cable attached to an Android device do `./run.sh` from the toplevel directory ...  it
  will compile, load, and launch the app on the device

Watch logcat
------------

- Watch the adb logcat for crash (on 4.3+ devices ... 4.2 appears immune)

