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

namespace wifi {
namespace ipc {

class IpcHandler {
public:
  virtual int32_t openIpc() = 0;

  virtual int32_t readIpc(uint8_t* aData, size_t aDataLen) = 0;

  virtual int32_t writeIpc(uint8_t* aData, size_t aDataLen) = 0;

  virtual int32_t closeIpc() = 0;

  virtual int32_t waitForData() = 0;

  virtual bool isConnected() = 0;

  virtual ~IpcHandler();
};
} // namespace ipc
} // namespace wifi
#endif // IpcHandler_h
