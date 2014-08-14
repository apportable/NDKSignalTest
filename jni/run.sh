#!/bin/sh

ndk-build NDK_DEBUG=1 && \
    ant -f ../build.xml debug install && \
    adb shell am start -a android.intent.action.MAIN -n com.apportable.ndksignaltest/.NDKSignalTest
