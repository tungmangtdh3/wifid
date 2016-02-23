/*
 * Copyright (C) 2015-2016  Mozilla Foundation
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

#ifndef WifiDebug_h
#define WifiDebug_h

#include "utils/Log.h"

#define FUNC __PRETTY_FUNCTION__

#define TAG_WIFID "wifid"

#ifdef WIFI_DEBUG_FLAG
#define WIFI_DEBUG(level, tag, msg, ...)                                 \
    __android_log_print(level, tag, "%s: " msg, FUNC, ##__VA_ARGS__)
#else
#define WIFI_DEBUG(level, tag, msg, ...)
#endif

#define WIFID_DEBUG(msg, ...)  \
  WIFI_DEBUG(ANDROID_LOG_DEBUG, TAG_WIFID, msg, ##__VA_ARGS__)
#define WIFID_WARNING(msg, ...)  \
  WIFI_DEBUG(ANDROID_LOG_WARN, TAG_WIFID, msg, ##__VA_ARGS__)
#define WIFID_ERROR(msg, ...)  \
  WIFI_DEBUG(ANDROID_LOG_ERROR, TAG_WIFID, msg, ##__VA_ARGS__)

#endif
