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
#ifndef MessageHandlerListener_h
#define MessageHandlerListener_h

#include "WifiMessage.h"

namespace wifi {

class MessageHandlerListener {
 public:
  virtual void OnNotification(WifiNotificationType aType,
                              void* aData,
                              size_t aLength) = 0;
  virtual void OnResponse(WifiMessageType aType,
                          WifiStatusCode aStatus,
                          void* aData,
                          size_t aLength) = 0;


  virtual ~MessageHandlerListener();
};
}
#endif // MessageHandlerListener_h
