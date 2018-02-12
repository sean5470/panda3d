/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file config_android.cxx
 * @author rdb
 * @date 2013-01-12
 */

#include "config_android.h"
#include "pnmFileTypeAndroid.h"
#include "pnmFileTypeRegistry.h"
#include "dconfig.h"
#include "pandaSystem.h"

NotifyCategoryDef(android, "");

struct android_app *panda_android_app = NULL;

jclass    jni_PandaActivity;
jmethodID jni_PandaActivity_readBitmapSize;
jmethodID jni_PandaActivity_readBitmap;
jmethodID jni_PandaActivity_showToast;

jclass   jni_BitmapFactory_Options;
jfieldID jni_BitmapFactory_Options_outWidth;
jfieldID jni_BitmapFactory_Options_outHeight;

/**
 * Initializes the library.  This must be called at least once before any of
 * the functions or classes in this library can be used.  Normally, this is
 * called by JNI_OnLoad.
 */
void
init_libandroid() {
  PNMFileTypeRegistry *tr = PNMFileTypeRegistry::get_global_ptr();
  PNMFileTypeAndroid::init_type();
  PNMFileTypeAndroid::register_with_read_factory();
  tr->register_type(new PNMFileTypeAndroid);
}

/**
 * Called by Java when loading this library.  Initializes the global class
 * references and the method IDs.
 */
jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
  init_libandroid();

  Thread *thread = Thread::get_current_thread();
  JNIEnv *env = thread->get_jni_env();
  nassertr(env != nullptr, -1);

  jni_PandaActivity = env->FindClass("org/panda3d/android/PandaActivity");
  jni_PandaActivity = (jclass) env->NewGlobalRef(jni_PandaActivity);

  jni_PandaActivity_readBitmapSize = env->GetStaticMethodID(jni_PandaActivity,
                   "readBitmapSize", "(J)Landroid/graphics/BitmapFactory$Options;");

  jni_PandaActivity_readBitmap = env->GetStaticMethodID(jni_PandaActivity,
                   "readBitmap", "(JI)Landroid/graphics/Bitmap;");

  jni_PandaActivity_showToast = env->GetMethodID(jni_PandaActivity,
                   "showToast", "(Ljava/lang/String;I)V");

  jni_BitmapFactory_Options = env->FindClass("android/graphics/BitmapFactory$Options");
  jni_BitmapFactory_Options = (jclass) env->NewGlobalRef(jni_BitmapFactory_Options);

  jni_BitmapFactory_Options_outWidth = env->GetFieldID(jni_BitmapFactory_Options, "outWidth", "I");
  jni_BitmapFactory_Options_outHeight = env->GetFieldID(jni_BitmapFactory_Options, "outHeight", "I");

  return JNI_VERSION_1_4;
}

/**
 * Called by Java when unloading this library.  Destroys the global class
 * references.
 */
void JNI_OnUnload(JavaVM *jvm, void *reserved) {
  Thread *thread = Thread::get_current_thread();
  JNIEnv *env = thread->get_jni_env();
  nassertv(env != nullptr);

  env->DeleteGlobalRef(jni_PandaActivity);
  env->DeleteGlobalRef(jni_BitmapFactory_Options);
}

/**
 * Shows a toast notification at the bottom of the activity.  The duration
 * should be 0 for short and 1 for long.
 */
void android_show_toast(ANativeActivity *activity, const string &message, int duration) {
  Thread *thread = Thread::get_current_thread();
  JNIEnv *env = thread->get_jni_env();
  nassertv(env != nullptr);

  jstring jmsg = env->NewStringUTF(message.c_str());
  env->CallVoidMethod(activity->clazz, jni_PandaActivity_showToast, jmsg, (jint)duration);
  env->DeleteLocalRef(jmsg);
}
