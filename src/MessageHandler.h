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

#ifndef MessageHandler_h
#define MessageHandler_h

#include "MessageHandlerListener.h"
#include "WifiGonkMessage.h"

namespace wifi {

class MessageProducer;
class WifiBaseMessage;

namespace controller
{
  class WpaSupplicantController;
  class HostApdController;
}

using namespace controller;

class MessageHandler : public MessageHandlerListener
{
public:
  ~MessageHandler();

  static MessageHandler& GetInstance();

  void Initialize(MessageProducer* aProducer);

  int Dispatch(uint8_t* aData, size_t aDataLen);

  void OnNotification(WifiNotificationType aType, void* aData, size_t aLength);
  void OnResponse(WifiMessageType aType, WifiStatusCode aStatus,
                         void* aData, size_t aLength);

private:
  MessageHandler();

  void HandleMessageVersion();

  void SendMessage(WifiBaseMessage* aMessage);
  void SendNotification(void* aNotification, size_t aLength);
  void SendResponse(WifiMessageType aType, WifiStatusCode aStatus);
  void SendResponse(WifiMessageType aType,
                    WifiStatusCode aStatus,
                    uint8_t* aData,
                    size_t aLength);


  static MessageHandler sInstance;
  MessageProducer* mProducer;
  std::map< uint16_t, std::queue<uint16_t> > mSessionMap;

  WpaSupplicantController* mWpaController;
  HostApdController* mHostApdController;
};

} //namespace wifi
#endif // MessageHandler_h
