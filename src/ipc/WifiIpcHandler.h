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

#ifndef WifiIpcHandler_h
#define WifiIpcHandler_h

#include "IpcHandler.h"

namespace wifi {
namespace ipc {

class WifiIpcHandler :
  public IpcHandler
{
public:
  static const int32_t CONNECT_MODE = 1;
  static const int32_t LISTEN_MODE  = 2;

  static const size_t NBOUNDS = 2; // respect leading and trailing '\0'

  WifiIpcHandler(int32_t aSockMode, const char* aSockName, bool aIsSeqPacket);
  ~WifiIpcHandler();

  int32_t openIpc();
  int32_t readIpc(uint8_t* aData, size_t aDataLen);
  int32_t writeIpc(uint8_t* aData, size_t aDataLen);
  int32_t closeIpc();
  int32_t waitForData();

  bool isConnected();

private:
  int32_t openConnectSocket();
  int32_t openListenSocket();
  void settingSocket();

  int32_t mRwFd;
  int32_t mConnFd;
  int32_t mSockMode;
  const char* mSockName;
  bool mIsSeqPacket;
  bool mIsConnected;
};
} //namespace ipc
} //namespace wifi
#endif // mozilla_WifiIpcHandler_h
