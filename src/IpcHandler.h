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

#ifndef IpcHandler_h
#define IpcHandler_h

#include <stdint.h>

class IpcHandler {
public:
  virtual int openIpc() = 0;

  virtual int readIpc(uint8_t* aData, size_t aDataLen) = 0;

  virtual int writeIpc(uint8_t* aData, size_t aDataLen) = 0;

  virtual int closeIpc() = 0;

  virtual int waitForData() = 0;

  virtual bool isConnected() = 0;

  virtual ~IpcHandler() = 0;
};

#endif // IpcHandler_h