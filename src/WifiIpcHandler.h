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

class WifiIpcHandler :
  public IpcHandler
{
public:
  static const int CONNECT_MODE = 1;
  static const int LISTEN_MODE  = 2;

  static const size_t NBOUNDS = 2; // respect leading and trailing '\0'

  WifiIpcHandler(int aSockMode, const char* aSockName, bool aIsSeqPacket);
  ~WifiIpcHandler();

  int openIpc();
  int readIpc(uint8_t* aData, size_t aDataLen);
  int writeIpc(uint8_t* aData, size_t aDataLen);
  int closeIpc();
  int waitForData();

  bool isConnected();

private:
  int openConnectSocket();
  int openListenSocket();
  void settingSocket();

  int mRwFd;
  int mConnFd;
  int mSockMode;
  const char* mSockName;
  bool mIsSeqPacket;
  bool mIsConnected;
};
} //namespace wifi
#endif // mozilla_WifiIpcHandler_h
