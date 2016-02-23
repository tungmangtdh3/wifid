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

#include <assert.h>

#include "IpcHandler.h"
#include "WifiDebug.h"
#include "WifiIpcManager.h"
#include "MessageHandler.h"

namespace wifi {

const size_t MAX_BUFSIZE = 4096;

WifiIpcManager WifiIpcManager::sInstance;

WifiIpcManager&
WifiIpcManager::GetInstance() {
   return sInstance;
}

WifiIpcManager::WifiIpcManager()
  : mIpcHandler(NULL)
{
}

WifiIpcManager::~WifiIpcManager()
{
}


void
WifiIpcManager::Initialize(IpcHandler* aIpcHandler, MessageProducer* aProducer)
{
  int ret;

  assert(aIpcHandler);

  mIpcHandler = aIpcHandler;

  MessageHandler::GetInstance().Initialize(aProducer);

  // open Socket
  ret = mIpcHandler->openIpc();

  if (ret < 0) {
    WIFID_ERROR("The initialization of the ipc manager fail.");
  }
}

void
WifiIpcManager::Loop()
{
  int ret;
  uint8_t buf[MAX_BUFSIZE];
  int length;
  while (1) {

    // open Socket
    ret = mIpcHandler->openIpc();

    if (ret < 0) {
      WIFID_ERROR("WifiIpcManager: Fail to open Ipc: %s\n", strerror(errno));
      continue;
    }

    while(mIpcHandler->isConnected()) {
      ret = mIpcHandler->waitForData();

      if (ret < 0) {
        WIFID_ERROR("WifiIpcManager: Error when waiting data: %s\n", strerror(errno));
        continue;
      }

      memset(buf, 0, MAX_BUFSIZE * sizeof(uint8_t));

      length = mIpcHandler->readIpc(buf, MAX_BUFSIZE);

      if (length == 0) {
        WIFID_DEBUG("WifiIpcManager: End of socket.");
        break;
      } else if (length < 0) {
        WIFID_ERROR("WifiIpcManager: Error when reading data: %s\n", strerror(errno));
        break;
      }

      ret = MessageHandler::GetInstance().Dispatch(buf, length);

      if (ret < 0) {
        WIFID_ERROR("WifiIpcManager: Error when processing data.\n");
      }
    }

    mIpcHandler->closeIpc();
  }
}

int WifiIpcManager::ConsumeMessage(WifiBaseMessage* aMessage)
{
  int ret = mIpcHandler->writeIpc(aMessage->GetBuffer(), aMessage->GetLength());
  delete aMessage;
  return ret;
}

} //namespace wifi
