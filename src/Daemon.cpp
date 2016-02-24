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
#include "IpcHandler.h"
#include "MessageConsumer.h"
#include "MessageProducer.h"
#include "MessageQueueWorker.h"
#include "WifiIpcHandler.h"
#include "IpcManager.h"

using namespace wifi::ipc;

namespace wifi {
namespace {

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

    mInitialized = false;
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

    mMsgQueueWorker = new MessageQueueWorker(&IpcManager::GetInstance());
    mMsgQueueWorker->Initialize();

    IpcManager::GetInstance().Initialize(mIpcHandler,mMsgQueueWorker);

    mInitialized = true;

    return true;
  }

  bool mInitialized;
  //TODO using unique_ptr and enable C++11
  IpcHandler*  mIpcHandler;
  IpcManager* mIpcManager;
  MessageQueueWorker* mMsgQueueWorker;
};
}  // namespace

bool Daemon::Initialize(const CommandLineOptions *aOp) {
  if (GetInstance().Init(aOp)) {
    return true;
  }
  return false;
}

void Daemon::ShutDown() {
}

Daemon& Daemon::GetInstance() {
  static DaemonImpl instance;
  return instance;
}

Daemon::~Daemon() {
}
}  // namespace wifi
