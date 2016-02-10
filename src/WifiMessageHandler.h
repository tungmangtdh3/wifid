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

#ifndef WifiMessageHandler_h
#define WifiMessageHandler_h

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <queue>
#include <map>

#include "WifiGonkMessage.h"
#include "WifiIpcManager.h"

#define WIFI_MSG_GET_HEADER(x) (reinterpret_cast<struct WifiMsgHeader*>(x))
#define WIFI_MSG_GET_CATEGORY(x) (WIFI_MSG_GET_HEADER(x)->msgCategory)
#define WIFI_MSG_GET_TYPE(x) (WIFI_MSG_GET_HEADER(x)->msgType)
#define WIFI_MSG_GET_LEN(x) (WIFI_MSG_GET_HEADER(x)->len)

#define WIFI_MSG_GET_REQ(x) (reinterpret_cast<struct WifiMsgReq*>(x))
#define WIFI_MSG_GET_REQ_SESSION_ID(x) (WIFI_MSG_GET_REQ(x)->sessionId)

class WifiMessageHandler
{
public:
  WifiMessageHandler();
  ~WifiMessageHandler();

  void setIpcManager(WifiIpcManager* aIpcMgr);
  int processMsg(uint8_t* aData, size_t aDataLen);
  int sendMsg(uint8_t* aData, size_t aDataLen);

  void processNotification(WifiNotificationType aType, void* aData, size_t aLength);
  void processResponse(WifiMessageType aType, WifiStatusCode aStatus,
                         void* aData, size_t aLength);

private:
  void handleMessageVersion();

  int sendNotificationEvent(void* aEventMsg, size_t aLength);
  int respondStatus(WifiMessageType aType, WifiStatusCode aStatus);

  WifiIpcManager* mIpcMgr;
  std::map< uint16_t, std::queue<uint16_t> > mSessionMap;
};

template<typename T>
class WifiEmptyMessage
{
public:
  WifiEmptyMessage(uint8_t* aData, size_t aDataLen)
    : mDataLength(aDataLen)
  {
    assert(aData);

    mData = new uint8_t[aDataLen];
    memcpy(mData, aData, aDataLen * sizeof(uint8_t));
    mMsg = reinterpret_cast<T*>(mData);
  }

  WifiEmptyMessage(WifiMessageCategory aMsgCategory, WifiMessageType aMsgType)
  {
    size_t length = sizeof(T);

    mData = new uint8_t[length];
    memset(mData, 0 , length * sizeof(uint8_t));
    mMsg = reinterpret_cast<T*>(mData);
    mMsg->hdr.msgCategory = aMsgCategory;
    mMsg->hdr.msgType = aMsgType;
    mMsg->hdr.len = length;
    mDataLength = length;
  }

  virtual ~WifiEmptyMessage()
  {
    delete mData;
  }

  uint8_t* getBuffer()
  {
    assert(mData);

    return mData;
  }

  size_t getLength()
  {
    return mDataLength;
  }

  T* operator->() const
  {
    assert(mMsg);

    return mMsg;
  }

private:
  T* mMsg;
  uint8_t* mData;
  size_t mDataLength;
};

template<typename T1, typename T2>
class WifiMessage
{
public:
  // Constructor for deserializing.
  WifiMessage(const uint8_t* aData, size_t aDataLen)
    : mDataLength(aDataLen)
  {
    assert(aData);

    mData = new uint8_t[aDataLen];
    memcpy(mData, aData, aDataLen * sizeof(uint8_t));
    mMsg = reinterpret_cast<T1*>(mData);
    mMsgBody = reinterpret_cast<T2*>(mData + sizeof(T1));
  }

  // Constructor for serializing with empty message body.
  WifiMessage(WifiMessageCategory aMsgCategory, WifiMessageType aMsgType)
  {
    size_t length = sizeof(T1) + sizeof(T2);

    mData = new uint8_t[length];
    memset(mData, 0 , length * sizeof(uint8_t));
    mMsg = reinterpret_cast<T1*>(mData);
    mMsg->hdr.msgCategory = aMsgCategory;
    mMsg->hdr.msgType = aMsgType;
    mMsg->hdr.len = length;
    mMsgBody = reinterpret_cast<T2*>(mData + sizeof(T1));
    mDataLength = length;
  }

  // Constructor for serializing with message.
  WifiMessage(WifiMessageCategory aMsgCategory, WifiMessageType aMsgType,
    const T2* aMsgBody, size_t aMsgLen)
    : mDataLength(aMsgLen)
  {
    size_t length;

    assert(aMsgBody);

    length = sizeof(T1) + aMsgLen;

    mData = new uint8_t[length];
    memset(mData, 0 , length * sizeof(uint8_t));
    mMsg = reinterpret_cast<T1*>(mData);
    mMsg->hdr.msgCategory = aMsgCategory;
    mMsg->hdr.msgType = aMsgType;
    mMsg->hdr.len = length;

    mMsgBody = reinterpret_cast<T2*>(mData + sizeof(T1));

    memcpy(mMsgBody, aMsgBody, aMsgLen * sizeof(uint8_t));
  }

  virtual ~WifiMessage()
  {
    delete mData;
  }

  uint8_t* getBuffer()
  {
    assert(mData);

    return mData;
  }

  size_t getLength()
  {
    return mDataLength;
  }

  T2* operator->() const
  {
    assert(mMsgBody);

    return mMsgBody;
  }

protected:
  T1* mMsg;
  T2* mMsgBody;
  uint8_t* mData;
  size_t mDataLength;
};

template<typename T>
class WifiRequestMessage
  : WifiMessage<struct WifiMsgReq, T>
{
public:
  WifiRequestMessage(const uint8_t* aData, size_t aDataLen)
    : WifiMessage<struct WifiMsgReq, T>(aData, aDataLen)
  {
  }

  WifiRequestMessage(WifiMessageType aMsgType)
    : WifiMessage<struct WifiMsgReq, T>(WIFI_MESSAGE_REQUEST, aMsgType)
  {
  }

  WifiRequestMessage(WifiMessageType aMsgType, const T* aMsgBody, size_t aMsgLen)
    : WifiMessage<struct WifiMsgReq, T>(WIFI_MESSAGE_REQUEST,
        aMsgType, aMsgBody, aMsgLen)
  {
  }

  ~WifiRequestMessage()
  {
  }

  uint16_t getSessionId()
  {
    return WifiMessage<struct WifiMsgReq, T>::mMsg->aSessionId;
  }

  void setSessionId(uint16_t aSessionId)
  {
    WifiMessage<struct WifiMsgReq, T>::mMsg->sessionId = aSessionId;
  }
};

template<typename T>
class WifiResponseMessage
  : public WifiMessage<struct WifiMsgResp, T>
{
public:
  WifiResponseMessage(const uint8_t* aData, size_t aDataLen)
    : WifiMessage<struct WifiMsgResp, T>(aData, aDataLen)
  {
  }

  WifiResponseMessage(WifiMessageType aMsgType)
    : WifiMessage<struct WifiMsgResp, T>(WIFI_MESSAGE_RESPONSE, aMsgType)
  {
  }

  WifiResponseMessage(WifiMessageType aMsgType, const T* aMsgBody, size_t aMsgLen)
    : WifiMessage<struct WifiMsgResp, T>(WIFI_MESSAGE_RESPONSE,
        aMsgType, aMsgBody, aMsgLen)
  {
  }

  ~WifiResponseMessage()
  {
  }

  uint16_t getSessionId()
  {
    return WifiMessage<struct WifiMsgResp, T>::mMsg->aSessionId;
  }

  void setSessionId(uint16_t aSessionId)
  {
    WifiMessage<struct WifiMsgResp, T>::mMsg->sessionId = aSessionId;
  }

  uint16_t getStatus()
  {
    return WifiMessage<struct WifiMsgResp, T>::mMsg->status;
  }

  void setStatus(WifiStatusCode aStatus)
  {
    WifiMessage<struct WifiMsgResp, T>::mMsg->status = aStatus;
  }
};

template<typename T>
class WifiNotificationMessage
  : public WifiMessage<struct WifiMsgNotify, T>
{
public:
  WifiNotificationMessage(const uint8_t* aData, size_t aDataLen)
    : WifiMessage<struct WifiMsgNotify, T>(aData, aDataLen)
  {
  }

  WifiNotificationMessage(WifiMessageType aMsgType)
    : WifiMessage<struct WifiMsgNotify, T>(WIFI_MESSAGE_NOTIFICATION, aMsgType)
  {
  }

  WifiNotificationMessage(WifiMessageType aMsgType, const T* aMsgBody, size_t aMsgLen)
    : WifiMessage<struct WifiMsgNotify, T>(WIFI_MESSAGE_NOTIFICATION,
        aMsgType, aMsgBody, aMsgLen)
  {
  }

  ~WifiNotificationMessage()
  {
  }
};

#endif // WifiMessageHandler_h