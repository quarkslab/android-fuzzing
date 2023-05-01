# Android greybox fuzzing with AFL++ Frida mode

This repository contains the material associated with the blogpost [Android greybox fuzzing with AFL++ Frida mode](https://blog.quarkslab.com/android-greybox-fuzzing-with-afl-frida-mode.html).

In particular:

- A [CMakeLists.txt](https://github.com/quarkslab/android-fuzzing/blob/main/AFLplusplus/CMakeLists.txt) file that allows to build AFL++ Frida mode for Android 12 aarch64;
- The [Android application](https://github.com/quarkslab/android-fuzzing/blob/main/apk/qb.blogfuzz.apk) used as the fuzzing target;
-  The [jenv library](https://github.com/quarkslab/android-fuzzing/tree/main/jenv) that allows the initialization of a Java environment from a native harness;
-  A standard native function [harness](https://github.com/quarkslab/android-fuzzing/tree/main/native);
-  A weakly linked JNI function [harness](https://github.com/quarkslab/android-fuzzing/tree/main/wlinked_jni);
-  A strongly linked JNI function [harness](https://github.com/quarkslab/android-fuzzing/tree/main/slinked_jni).

Information about the build and the use of this material is in the [blogpost](https://blog.quarkslab.com/android-greybox-fuzzing-with-afl-frida-mode.html).
