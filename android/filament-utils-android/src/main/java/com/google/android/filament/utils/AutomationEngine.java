/*
 * Copyright (C) 2021 The Android Open Source Project
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

package com.google.android.filament.utils;

import androidx.annotation.NonNull;

public class AutomationEngine {
    private long mNativeObject;

    /**
     * Allows users to toggle screenshots, change the sleep duration between tests, etc.
     */
    public static class Options {
        /**
         * Minimum time that the engine waits between applying a settings object and subsequently
         * taking a screenshot. After the screenshot is taken, the engine immediately advances to
         * the next test case. Specified in seconds.
         */
        float sleepDuration = 0.2f;

        /**
         * Similar to sleepDuration, but expressed as a frame count. Both the minimum sleep time
         * and the minimum frame count must be elapsed before the engine advances to the next test.
         */
        int minFrameCount = 2;

        /**
         * If true, test progress is dumped to the utils Log (info priority).
         */
        boolean verbose = true;
    }

    /**
     * Creates an automation engine from a JSON specification.
     *
     * @param jsonSpec Valid JSON string that conforms to the automation schema.
     */
    public AutomationEngine(@NonNull String jsonSpec) {
        mNativeObject = nCreateAutomationEngine(jsonSpec);
        if (mNativeObject == 0) throw new IllegalStateException("Couldn't create AutomationEngine");
    }

    /**
     * Creates an automation engine for the default test sequence.
     *
     * To see how the default test sequence is generated, search for DEFAULT_AUTOMATION.
     */
    public AutomationEngine() {
        mNativeObject = nCreateDefaultAutomationEngine();
        if (mNativeObject == 0) throw new IllegalStateException("Couldn't create AutomationEngine");
    }

    @Override
    protected void finalize() throws Throwable {
        nDestroyAutomationEngine(mNativeObject);
        super.finalize();
    }

    private static native void nDestroyAutomationEngine(long nativeObject);
    private static native long nCreateAutomationEngine(String jsonSpec);
    private static native long nCreateDefaultAutomationEngine();
}
