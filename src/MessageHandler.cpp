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

#include <assert.h>
#include <string.h>
#include <sys/socket.h>

#include "HostApdController.h"
#include "MessageProducer.h"
#include "WifiDebug.h"
#include "MessageHandler.h"
#include "WpaSupplicantController.h"

#define MAJOR_VER 1
#define MINOR_VER 0

using namespace controller;

namespace wifi {
MessageHandler MessageHandler::sInstance;

MessageHandler&
MessageHandler::GetInstance() {
   return sInstance;
}

MessageHandler::MessageHandler()
  : mProducer(NULL)
{
}

MessageHandler::~MessageHandler()
{
  delete mWpaController;
  delete mHostApdController;
}

void MessageHandler::Initialize(MessageProducer* aProducer) {
  mProducer = aProducer;

  //Wifi daemon works based on request-response mechanism
  //and there should be no race condition.
  //TODO handle race condition if necessary 
  mWpaController =
    new WpaSupplicantController(static_cast<MessageHandlerListener*>(this));
  mHostApdController = new HostApdController(static_cast<MessageHandlerListener*>(this));
}

int
MessageHandler::Dispatch(uint8_t* aData, size_t aDataLen)
{
  assert(aData);

  uint16_t msgCategory;
  uint16_t msgType;
  uint16_t sessionId;

  msgCategory = WIFI_MSG_GET_CATEGORY(aData);

  if (msgCategory != WIFI_MESSAGE_REQUEST) {
    WIFID_WARNING("The wifi daemon only process request message. Message: %d.",
      msgCategory);
    return 0;
  }

  msgType = WIFI_MSG_GET_TYPE(aData);
  sessionId = WIFI_MSG_GET_REQ_SESSION_ID(aData);

  // Insert session id into session map according to the message type.
  mSessionMap[msgType].push(sessionId);

  switch (msgType) {
    case WIFI_MESSAGE_TYPE_VERSION:
      HandleMessageVersion();
      break;

    case WIFI_MESSAGE_TYPE_LOAD_DRIVER:
    case WIFI_MESSAGE_TYPE_UNLOAD_DRIVER:
    case WIFI_MESSAGE_TYPE_START_SUPPLICANT:
    case WIFI_MESSAGE_TYPE_STOP_SUPPLICANT:
    case WIFI_MESSAGE_TYPE_CONNECT_TO_SUPPLICANT:
    case WIFI_MESSAGE_TYPE_CLOSE_SUPPLICANT_CONNECTION:
    case WIFI_MESSAGE_TYPE_COMMAND:
      mWpaController->HandleRequest(msgType, aData, aDataLen);
      break;

    default:
      break;
  }

  return 0;
}

void
MessageHandler::OnNotification(WifiNotificationType aType,
  void* aData, size_t aLength)
{
  switch (aType) {
    case WIFI_NOTIFICATION_EVENT:
        SendNotification(aData, aLength);
      break;

    default:
      WIFID_ERROR("Notification Type(%d) does not support.", aType);
      break;
  }
}

void
MessageHandler::OnResponse(
  WifiMessageType aType,
  WifiStatusCode aStatus,
  void* aData,
  size_t aLength)
{
  switch (aType) {
    case WIFI_MESSAGE_TYPE_LOAD_DRIVER:
    case WIFI_MESSAGE_TYPE_UNLOAD_DRIVER:
    case WIFI_MESSAGE_TYPE_START_SUPPLICANT:
    case WIFI_MESSAGE_TYPE_STOP_SUPPLICANT:
    case WIFI_MESSAGE_TYPE_CONNECT_TO_SUPPLICANT:
    case WIFI_MESSAGE_TYPE_CLOSE_SUPPLICANT_CONNECTION:
      SendResponse(aType, aStatus);
      break;

    case WIFI_MESSAGE_TYPE_COMMAND:
      //TODO add reply and len
      SendResponse(WIFI_MESSAGE_TYPE_COMMAND, aStatus,static_cast<uint8_t*>(aData), aLength);
      break;

    default:
      WIFID_ERROR("Response Type(%d) does not support.", aType);
      break;
  }
}

void
MessageHandler::SendMessage(WifiBaseMessage* aMessage)
{
  assert(aData);

  mProducer->PushMessage(aMessage);
}

void
MessageHandler::SendNotification(void* aNotification, size_t aLength)
{
  WifiBaseMessage* notifyMsg = new WifiNotificationMessage<WifiMsgNotifyEvent>
    (reinterpret_cast<uint8_t*>(aNotification), aLength);

  SendMessage(notifyMsg);
}

void
MessageHandler::SendResponse(WifiMessageType aType, WifiStatusCode aStatus)
{
  uint16_t sessionId;

  sessionId = mSessionMap[aType].front();
  mSessionMap[aType].pop();

  WifiEmptyMessage<WifiMsgResp>* respMsg = new
    WifiEmptyMessage<WifiMsgResp>(WIFI_MESSAGE_RESPONSE, aType);
  (*respMsg)->sessionId = sessionId;
  (*respMsg)->status = aStatus;

  SendMessage(static_cast<WifiBaseMessage*>(respMsg));
}

void
MessageHandler::SendResponse(WifiMessageType aType,
                                WifiStatusCode aStatus,
                                uint8_t* aData,
                                size_t aLength)
{
  //TODO send response with data structured in WifiMessage
  uint16_t sessionId;

  sessionId = mSessionMap[aType].front();
  mSessionMap[aType].pop();

  WifiEmptyMessage<WifiMsgResp>* respMsg = new
    WifiEmptyMessage<WifiMsgResp>(aData, aLength);

  SendMessage(static_cast<WifiBaseMessage*>(respMsg));
}

void
MessageHandler::HandleMessageVersion()
{
  uint16_t sessionId;

  sessionId = mSessionMap[WIFI_MESSAGE_TYPE_VERSION].front();
  mSessionMap[WIFI_MESSAGE_TYPE_VERSION].pop();

  WifiResponseMessage<WifiMsgVersion>* respMsg =
    new WifiResponseMessage<WifiMsgVersion>(WIFI_MESSAGE_TYPE_VERSION);

  respMsg->SetStatus(WIFI_STATUS_OK);
  respMsg->SetSessionId(sessionId);
  (*respMsg)->majorVersion = MAJOR_VER;
  (*respMsg)->minorVersion = MINOR_VER;

  SendMessage(static_cast<WifiBaseMessage*>(respMsg));
}

} //namespace wifi
