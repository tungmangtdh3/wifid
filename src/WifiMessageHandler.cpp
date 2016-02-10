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

#include "WifiDebug.h"
#include "WifiMessageHandler.h"

#define MAJOR_VER 1
#define MINOR_VER 0

WifiMessageHandler::WifiMessageHandler()
  : mIpcMgr(NULL)
{
}

WifiMessageHandler::~WifiMessageHandler()
{
}

void
WifiMessageHandler::setIpcManager(WifiIpcManager* aIpcMgr)
{
  assert(aIpcMgr);

  mIpcMgr = aIpcMgr;
}

int
WifiMessageHandler::processMsg(uint8_t* aData, size_t aDataLen)
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
      handleMessageVersion();
      break;

    case WIFI_MESSAGE_TYPE_LOAD_DRIVER:
      break;

    case WIFI_MESSAGE_TYPE_UNLOAD_DRIVER:
      break;

    case WIFI_MESSAGE_TYPE_START_SUPPLICANT:
      break;

    case WIFI_MESSAGE_TYPE_STOP_SUPPLICANT:
      break;

    case WIFI_MESSAGE_TYPE_CONNECT_TO_SUPPLICANT:
      break;

    case WIFI_MESSAGE_TYPE_CLOSE_SUPPLICANT_CONNECTION:
      break;

    case WIFI_MESSAGE_TYPE_COMMAND:
      break;

    default:
      break;
  }

  return 0;
}

void
WifiMessageHandler::processNotification(WifiNotificationType aType,
  void* aData, size_t aLength)
{
  switch (aType) {
    case WIFI_NOTIFICATION_EVENT:
        sendNotificationEvent(aData, aLength);
      break;

    default:
      WIFID_ERROR("Notification Type(%d) does not support.", aType);
      break;
  }
}

void
WifiMessageHandler::processResponse(
  WifiMessageType aType,
  WifiStatusCode aStatus,
  void* aData,
  size_t aLength)
{
  switch (aType) {

    case WIFI_MESSAGE_TYPE_LOAD_DRIVER:
      respondStatus(WIFI_MESSAGE_TYPE_LOAD_DRIVER, aStatus);
      break;

    case WIFI_MESSAGE_TYPE_UNLOAD_DRIVER:
      respondStatus(WIFI_MESSAGE_TYPE_UNLOAD_DRIVER, aStatus);
      break;

    case WIFI_MESSAGE_TYPE_START_SUPPLICANT:
      respondStatus(WIFI_MESSAGE_TYPE_START_SUPPLICANT, aStatus);
      break;

    case WIFI_MESSAGE_TYPE_STOP_SUPPLICANT:
      respondStatus(WIFI_MESSAGE_TYPE_STOP_SUPPLICANT, aStatus);
      break;

    case WIFI_MESSAGE_TYPE_CONNECT_TO_SUPPLICANT:
      respondStatus(WIFI_MESSAGE_TYPE_CONNECT_TO_SUPPLICANT, aStatus);
      break;

    case WIFI_MESSAGE_TYPE_CLOSE_SUPPLICANT_CONNECTION:
      respondStatus(WIFI_MESSAGE_TYPE_CLOSE_SUPPLICANT_CONNECTION, aStatus);
      break;

    case WIFI_MESSAGE_TYPE_COMMAND:
      respondStatus(WIFI_MESSAGE_TYPE_COMMAND, aStatus);
      break;

    default:
      WIFID_ERROR("Response Type(%d) does not support.", aType);
      break;
  }
}

int
WifiMessageHandler::sendMsg(uint8_t* aData, size_t aDataLen)
{
  assert(aData);

  return mIpcMgr->writeToIpc(aData, aDataLen);
}

int
WifiMessageHandler::sendNotificationEvent(void* aEventMsg, size_t aLength)
{
  int ret;

  WifiNotificationMessage<struct WifiMsgNotifyEvent> notifyMsg(
    reinterpret_cast<uint8_t*>(aEventMsg), aLength);

  ret = sendMsg(notifyMsg.getBuffer(), notifyMsg.getLength());

  if (ret < 0) {
    WIFID_ERROR("Fail on sending the notification(%s).", strerror(errno));
  }

  return ret;
}

int
WifiMessageHandler::respondStatus(WifiMessageType aType, WifiStatusCode aStatus)
{
  int ret;
  uint16_t sessionId;

  sessionId = mSessionMap[aType].front();
  mSessionMap[aType].pop();

  WifiEmptyMessage<struct WifiMsgResp> respMsg(WIFI_MESSAGE_RESPONSE, aType);
  respMsg->sessionId = sessionId;
  respMsg->status = aStatus;

  ret = sendMsg(respMsg.getBuffer(), respMsg.getLength());

  if (ret < 0) {
    WIFID_ERROR("Fail on responding the message(%s).", strerror(errno));
  }

  return ret;
}

void
WifiMessageHandler::handleMessageVersion()
{
  int ret;
  uint16_t sessionId;

  sessionId = mSessionMap[WIFI_MESSAGE_TYPE_VERSION].front();
  mSessionMap[WIFI_MESSAGE_TYPE_VERSION].pop();

  WifiResponseMessage<struct WifiMsgVersion> respMsg(WIFI_MESSAGE_TYPE_VERSION);

  respMsg.setStatus(WIFI_STATUS_OK);
  respMsg.setSessionId(sessionId);
  respMsg->majorVersion = MAJOR_VER;
  respMsg->minorVersion = MINOR_VER;

  ret = sendMsg(respMsg.getBuffer(), respMsg.getLength());

  if (ret < 0) {
    WIFID_ERROR("Fail on responding the message of getting version(%s).", strerror(errno));
  }
}
