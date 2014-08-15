NDKSignalTest
=============

- Shows a crash on Android 4.3+ devices (Nexus 7 devices tested).

- This may be interesting to Android developers working on libc (bionic) and/or Linux kernel

Build a reduced testcase as a standard Android app
--------------------------------------------------

- Edit local.properties to specify path to Android NDK/SDK so you can build this

- With a USB cable attached to an Android device do `./run.sh` from the toplevel directory ...  it
  will compile, load, and launch the app on the device

Watch logcat
------------

- Watch the adb logcat for crash (on 4.3+ devices ... 4.2 appears immune)

To build a super-reduced ADB commandline testcase
-------------------------------------------------

YMMV...

- Edit the `jni/Android.mk` file to comment the `include $(BUILD_SHARED_LIBRARY)` line and uncomment the `include $(BUILD_EXECUTABLE)` line

- Edit the `jni/NDKSignalTest.c` file to enable the main() codepath ... you may also need to increate the number of
  native worker threads

- Execute `ndk-build` in the jni directory

- Copy the resulting exe using ADB to an executable directory device, e.g. for a Nexus 7 running
  Android 4.2+ : `adb push ../libs/armeabi/NDKSignalTest /data/local/tmp/`

- Possibly you may need to add executable permissions : `adb shell chmod 755 /data/local/tmp/NDKSignalTest`

- Execute over adb : `adb shell /data/local/tmp/NDKSignalTest`

