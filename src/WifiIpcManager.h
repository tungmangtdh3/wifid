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

#ifndef WifiIpcManager_h
#define WifiIpcManager_h

#include <stdint.h>

#include "IpcHandler.h"

class WifiMessageHandler;

class WifiIpcManager
{
private:
  static WifiIpcManager* sInstance;

public:
  ~WifiIpcManager();

  static WifiIpcManager* Instance();

  void init(IpcHandler* aIpcHandler, WifiMessageHandler* aMsgHandler);
  void loop();
  int writeToIpc(uint8_t* aData, size_t aDataLen);

private:
  WifiIpcManager();

  IpcHandler*     mIpcHandler;
  WifiMessageHandler* mMsgHandler;
};

#endif // WifiIpcManager_h