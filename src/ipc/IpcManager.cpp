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
#include "IpcManager.h"
#include "MessageHandler.h"

namespace wifi {
namespace ipc {

const size_t MAX_BUFSIZE = 4096;

IpcManager IpcManager::sInstance;

IpcManager&
IpcManager::GetInstance() {
   return sInstance;
}

IpcManager::IpcManager()
  : mIpcHandler(NULL)
{
}

IpcManager::~IpcManager()
{
}


void IpcManager::Initialize(IpcHandler* aIpcHandler, MessageProducer* aProducer)
{
  int32_t ret;

  assert(aIpcHandler);

  mIpcHandler = aIpcHandler;

  MessageHandler::GetInstance().Initialize(aProducer);

  // open Socket
  ret = mIpcHandler->openIpc();

  if (ret < 0) {
    WIFID_ERROR("The initialization of the ipc manager fail.");
  }
}

void IpcManager::Loop()
{
  int32_t ret;
  uint8_t buf[MAX_BUFSIZE];
  int32_t length;
  while (1) {

    // open Socket
    ret = mIpcHandler->openIpc();

    if (ret < 0) {
      WIFID_ERROR("IpcManager: Fail to open Ipc: %s\n", strerror(errno));
      continue;
    }

    while(mIpcHandler->isConnected()) {
      ret = mIpcHandler->waitForData();

      if (ret < 0) {
        WIFID_ERROR("IpcManager: Error when waiting data: %s\n", strerror(errno));
        continue;
      }

      memset(buf, 0, MAX_BUFSIZE * sizeof(uint8_t));

      length = mIpcHandler->readIpc(buf, MAX_BUFSIZE);

      if (length == 0) {
        WIFID_DEBUG("IpcManager: End of socket.");
        break;
      } else if (length < 0) {
        WIFID_ERROR("IpcManager: Error when reading data: %s\n", strerror(errno));
        break;
      }

      ret = MessageHandler::GetInstance().Dispatch(buf, length);

      if (ret < 0) {
        WIFID_ERROR("IpcManager: Error when processing data.\n");
      }
    }

    mIpcHandler->closeIpc();
  }
}

int32_t IpcManager::ConsumeMessage(WifiBaseMessage* aMessage)
{
  int32_t ret = mIpcHandler->writeIpc(aMessage->GetBuffer(), aMessage->GetLength());
  delete aMessage;
  return ret;
}
} //namespace ipc
} //namespace wifi
