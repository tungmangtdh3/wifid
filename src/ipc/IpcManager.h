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

#ifndef IpcManager_h
#define IpcManager_h

#include "MessageConsumer.h"

using namespace wifi;

namespace wifi {
class MessageProducer;
class WifiBaseMessage;
class MessageDispatcher;
}

namespace wifi {
namespace ipc {

class IpcHandler;

class IpcManager : public MessageConsumer
{
private:
  static IpcManager sInstance;

public:
  ~IpcManager();

  static IpcManager& GetInstance();

  void Initialize(IpcHandler* aIpcHandler, MessageProducer* aProducer);

  void ShutDown();

  void Loop();

  int32_t ConsumeMessage(WifiBaseMessage* aMessage);

private:
  IpcManager();

  IpcHandler*     mIpcHandler;
};
} //ipc
} //wifi
#endif // IpcManager_h
