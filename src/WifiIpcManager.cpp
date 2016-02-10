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

#include "WifiDebug.h"
#include "WifiIpcManager.h"
#include "WifiMessageHandler.h"

const size_t MAX_BUFSIZE = 4096;

WifiIpcManager* WifiIpcManager::sInstance = NULL;

WifiIpcManager*
WifiIpcManager::Instance() {
  if (!sInstance) {
    sInstance = new WifiIpcManager();
  }
  return sInstance;
}

WifiIpcManager::WifiIpcManager()
  : mIpcHandler(NULL)
  , mMsgHandler(NULL)
{
}

WifiIpcManager::~WifiIpcManager()
{
}

void
WifiIpcManager::init(IpcHandler* aIpcHandler, WifiMessageHandler* aMsgHandler)
{
  int ret;

  assert(aIpcHandler);
  assert(aMsgHandler);

  mIpcHandler = aIpcHandler;
  mMsgHandler = aMsgHandler;

  // open Socket
  ret = mIpcHandler->openIpc();

  if (ret < 0) {
    WIFID_ERROR("The initialization of the ipc manager fail.");
  }
}

void
WifiIpcManager::loop()
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

      ret = mMsgHandler->processMsg(buf, length);

      if (ret < 0) {
        WIFID_ERROR("WifiIpcManager: Error when processing data.\n");
      }
    }

    mIpcHandler->closeIpc();
  }
}

int
WifiIpcManager::writeToIpc(uint8_t* aData, size_t aDataLen)
{
  if (aData == NULL) {
    return -1;
  }

  return mIpcHandler->writeIpc(aData, aDataLen);
}