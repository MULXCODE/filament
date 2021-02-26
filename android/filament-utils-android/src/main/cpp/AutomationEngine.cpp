/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>

#include <viewer/AutomationEngine.h>

using namespace filament::viewer;

extern "C" JNIEXPORT jlong JNICALL
Java_com_google_android_filament_utils_AutomationEngine_nCreateAutomationEngine(JNIEnv* env, jclass,
        jstring spec_) {
    const char* spec = env->GetStringUTFChars(spec_, 0);
    jlong result = (jlong) AutomationEngine::createFromJSON(spec, strlen(spec));
    env->ReleaseStringUTFChars(spec_, spec);
    return result;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_google_android_filament_utils_AutomationEngine_nCreateDefaultAutomationEngine(JNIEnv* env,
        jclass klass) {
    return (jlong) AutomationEngine::createDefaultTest();
}

extern "C" JNIEXPORT void JNICALL
Java_com_google_android_filament_ColorGrading_nDestroyAutomationEngine(JNIEnv* env, jclass klass,
        jlong nativeAutomation) {
    AutomationEngine* automation = (AutomationEngine*) nativeAutomation;
    delete automation;
}
