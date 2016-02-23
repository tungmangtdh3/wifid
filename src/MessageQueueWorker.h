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

#ifndef MessageQueueWorker_h
#define MessageQueueWorker_h

#include <vector>
#include "MessageProducer.h"

namespace wifi {

class MessageConsumer;
class WifiBaseMessage;

class MessageQueueWorker : public MessageProducer
{
public:
  void Initialize();

  void* Loop();

  void ShutDown();

  void PushMessage(const WifiBaseMessage* aMessage);

  MessageQueueWorker(MessageConsumer* aConsummer);

  ~MessageQueueWorker();

private:

  //TODO currently using vector to store messages
  //May use a LIFO queue
  //Should use unique_ptr of C++11
  std::vector <WifiBaseMessage*> mMessageQueue;

  MessageConsumer* mConsumer;

  bool mDone;

};
}
#endif // MessageQueueWorker_h
