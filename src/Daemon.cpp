/*
 * Copyright (C) 2014-2015  Mozilla Foundation
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

#include <memory>
#include <stdlib.h>

#include "Daemon.h"
#include "MessageConsumer.h"
#include "MessageProducer.h"
#include "MessageQueueWorker.h"
#include "WifiIpcHandler.h"
#include "WifiIpcManager.h"

namespace wifi {
namespace {

// The global Daemon instance.
Daemon* sDaemon = NULL;

class DaemonImpl : public Daemon {
 public:
  DaemonImpl() : mInitialized(false) {
  }

  ~DaemonImpl(){
    if (!mInitialized)
      return;

    CleanUp();
  }

  void Start() {
    mIpcManager->Loop();
  }

 private:
  void CleanUp() {
    delete mIpcHandler;
    mMsgQueueWorker->ShutDown();
    delete mMsgQueueWorker;
  }

  bool Init(const CommandLineOptions *aOp) {
    if (mInitialized) {
      return false;
    }

    mIpcHandler = new WifiIpcHandler(aOp->useListenSocket ?
                                           WifiIpcHandler::LISTEN_MODE :
                                           WifiIpcHandler::CONNECT_MODE,
                                         aOp->socketName,
                                         aOp->useSeqPacket);

    mMsgQueueWorker = new MessageQueueWorker(static_cast<MessageConsumer*>(&WifiIpcManager::GetInstance()));
    mMsgQueueWorker->Initialize();

    WifiIpcManager::GetInstance().Initialize(mIpcHandler,static_cast<MessageProducer*>(mMsgQueueWorker));

    return true;
  }

  bool mInitialized;
  //TODO using unique_ptr and enable C++11
  WifiIpcHandler*  mIpcHandler;
  WifiIpcManager* mIpcManager;
  MessageQueueWorker* mMsgQueueWorker;
  
};
}  // namespace

bool Daemon::Initialize(const CommandLineOptions *aOp) {
  if (!sDaemon) {
    return false;
  }

  sDaemon = new DaemonImpl();
  if (sDaemon->Init(aOp)) {
    return true;
  }

  delete sDaemon;
  sDaemon = NULL;

  return false;
}

void Daemon::ShutDown() {
  if (sDaemon) {
    delete sDaemon;
    sDaemon = NULL;
  }
}

Daemon* Daemon::GetInstance() {
  return sDaemon;
}

Daemon::~Daemon() {
}
}  // namespace wifi
