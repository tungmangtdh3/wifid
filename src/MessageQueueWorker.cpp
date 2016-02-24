/*
 * Copyright (C) 2013-2014  Mozilla Foundation
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

#include <pthread.h>
#include <semaphore.h>

#include "MessageConsumer.h"
#include "MessageQueueWorker.h"
#include "WifiDebug.h"
#include "WifiMessage.h"

namespace wifi {

static pthread_t sThread;
static sem_t sSem;

MessageQueueWorker::MessageQueueWorker(MessageConsumer* aConsumer)
  : mConsumer(aConsumer)
{
}

MessageQueueWorker::~MessageQueueWorker()
{
  if (!mMessageQueue.empty()) {
    std::vector<WifiBaseMessage*>::iterator i = mMessageQueue.begin();
    for (; i != mMessageQueue.end(); i++)
      delete *i;
    mMessageQueue.clear();
  }
}

static void* StaticThreadFunc(void* aArg)
{
  pthread_setname_np(pthread_self(), "MessageQueueWorker thread");
  MessageQueueWorker* queue = reinterpret_cast<MessageQueueWorker*>(aArg);
  return queue->Loop();
}

void MessageQueueWorker::Initialize()
{
  if (sem_init(&sSem, 0, 0) == -1) {
    abort();
  }

  mDone = true;

  if (pthread_create(&sThread, NULL, StaticThreadFunc, this) != 0) {
    abort();
  }
}

void MessageQueueWorker::ShutDown()
{
  //TODO worker shutdowns immediately and no waiting join the thread.
  //It should wait for consuming message completely
  mDone = false;
}

void* MessageQueueWorker::Loop()
{
  while(mDone) {
    if(sem_wait(&sSem)) {
      abort();
    }

    while (!mMessageQueue.empty()) {
      WifiBaseMessage* message = *mMessageQueue.begin();
      mMessageQueue.erase(mMessageQueue.begin());
      mConsumer->ConsumeMessage(message);
    }
  }
  return NULL;
}

void MessageQueueWorker::PushMessage(const WifiBaseMessage* aMessage) {
  mMessageQueue.push_back(const_cast<WifiBaseMessage*>(aMessage));
  sem_post(&sSem);
}
} //namespace wifi
