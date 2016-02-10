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

#include <cutils/log.h>

#include "wifid.h"
#include "WifiDebug.h"
#include "WifiGonkMessage.h"
#include "WifiMessageHandler.h"
#include "WifiIpcHandler.h"
#include "WifiIpcManager.h"

#define LOG_TAG "wifid"

const char* SOCKNAME = "wifid";

bool gWifiDebugFlag = true;

int main() {

  // Create the wifi ipc handler
  WifiIpcHandler* ipcHandler = new WifiIpcHandler(WifiIpcHandler::CONNECT_MODE, SOCKNAME, true);

  // Create the wifi message handler
  WifiMessageHandler* msgHandler = new WifiMessageHandler();

  // Create the Ipc manager
  WifiIpcManager* ipcManager = WifiIpcManager::Instance();

  // Initiate
  ipcManager->init(ipcHandler, msgHandler);
  msgHandler->setIpcManager(ipcManager);



}