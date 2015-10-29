NDKSignalTest
=============

- Shows a crash on Android 4.2+ devices (mostly Nexus devices tested)

- This may be interesting to Android developers working on libc (bionic) and/or Linux kernel

Build a reduced testcase as a standard Android app
--------------------------------------------------

- Edit local.properties to specify path to Android NDK/SDK so you can build this

- With a USB cable attached to an Android device do `./run.sh` from the toplevel directory ...  it
  will compile, load, and launch the app on the device

Watch logcat
------------

- Watch the adb logcat for crash (see list of devices known to crash below)

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

Crashing Devices
----------------

Serial number is reported by `adb devices`

- Nexus 5         running 4.4.4 ... Kernel 3.4.0-gd59db4e      android-build@vpbs1.mtv.corp.google.com #1 Mon Mar 17 15:16:36 PDT 2014  (serial # 05efecfcf0db3246)

- Nexus 5         running 5.0   ... Kernel 3.4.0-g323bbd9      android-build@vpbs1.mtv.corp.google.com (gcc version 4.7 (GCC) ) #1 SMP PREEMPT Thu Oct 9 05:57:15 UTC 2014 (serial #03758e42437e9654)

- Nexus 4         running 4.2.2 ... Kernel 3.4.0-perf-g7ce11cd android-build@vpbs1.mtv.corp.google.com #1 Tue Jan 29 11:41:33 PST 2013  (serial # 04c79daee4d3951f)

Apparently Immune Devices
-------------------------

- Nexus 7         running 4.2.2 ... Kernel 3.1.10-g05b777c     android-build@vpbs1.mtv.corp.google.com #1 Thu Nov 29 10:35:37 PST 2012  (serial # 015d1244cd5bf40c)

- Nexus 7         running 4.3   ... Kernel 3.1.10-g1e8b3d8     android-build@vpbs1.mtv.corp.google.com #1 Tue Jun 11 23:51:41 PDT 2013  (serial # 015d2109ab1ffc03)

- Nexus 7         running 4.4.4 ... Kernel 3.1.10-g1e42d16     android-build@vpbs1.mtv.corp.google.com #1 Thu Mar 13 11:33:27 PDT 2014  (serial # 015d2d42235c0808)

- Samsung GT4 10" running 4.4.2 ... Kernel 3.4.0-1105486 dpi@SWDD5006 #1                                  Sat Mar 29 11:06:39 KST 2014  (serial # 51d5881002d21796)

- Nexus 10        running 4.2.2 ... Kernel 3.4.5-gaf9c307      android-build@vpbs1.mtv.corp.google.com #1 Tue Jan  8 17:22:07 PST 2013  (serial # R32CA04LS1D)

- Nexus 10        running 4.4.2 ... Kernel 3.4.39-g5b5c8df     android-build@vpbs1.mtv.corp.google.com #1 Wed Nov 20 15:12:05 PST 2013  (serial # R32CB017DRT)
