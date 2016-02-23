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

#ifndef WifiGonkMessage_h
#define WifiGonkMessage_h

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <queue>
#include <map>

#include "WifiGonkMessage.h"

#define WIFI_MSG_GET_HEADER(x) (reinterpret_cast<struct WifiMsgHeader*>(x))
#define WIFI_MSG_GET_CATEGORY(x) (WIFI_MSG_GET_HEADER(x)->msgCategory)
#define WIFI_MSG_GET_TYPE(x) (WIFI_MSG_GET_HEADER(x)->msgType)
#define WIFI_MSG_GET_LEN(x) (WIFI_MSG_GET_HEADER(x)->len)

#define WIFI_MSG_GET_REQ(x) (reinterpret_cast<struct WifiMsgReq*>(x))
#define WIFI_MSG_GET_REQ_SESSION_ID(x) (WIFI_MSG_GET_REQ(x)->sessionId)

namespace wifi {

/**
 * WiFi daemon message format
 *
 * 2 bytes of a message category. (Request/Response/Notification)
 * 2 bytes of a message type. (Depending on category)
 * 4 bytes of the length of this message. (Exclude first 8 bytes)
 * Payload of this message with various lengths depends on message type.
 *
 * Payload of a request message
 *   2 bytes of the session Id.
 *   Data of the request message.
 *
 * Payload of a response messages
 *   2 bytes of the session Id.
 *   2 bytes of status code.
 *   Data of the response message.
 *
 * Payload of a notification message
 *   Data of the notification message.
 */

/**
 * Message categories.
 */
typedef enum {
  WIFI_MESSAGE_REQUEST = 0,
  WIFI_MESSAGE_RESPONSE = 1,
  WIFI_MESSAGE_NOTIFICATION = 2,
} WifiMessageCategory;

/**
 * WiFi Message Types.
 */
typedef enum {
  WIFI_MESSAGE_TYPE_VERSION,
  WIFI_MESSAGE_TYPE_LOAD_DRIVER,
  WIFI_MESSAGE_TYPE_UNLOAD_DRIVER,
  WIFI_MESSAGE_TYPE_START_SUPPLICANT,
  WIFI_MESSAGE_TYPE_STOP_SUPPLICANT,
  WIFI_MESSAGE_TYPE_CONNECT_TO_SUPPLICANT,
  WIFI_MESSAGE_TYPE_CLOSE_SUPPLICANT_CONNECTION,
  WIFI_MESSAGE_TYPE_COMMAND,
  WIFI_MESSAGE_TYPE_CONNECT_TO_HOSTAPD,
  WIFI_MESSAGE_TYPE_CLOSE_HOSTAPD_CONNECTION,
  WIFI_MESSAGE_TYPE_HOSTAPD_GET_STATIONS,
  WIFI_MESSAGE_TYPE_HOSTAPD_COMMAND
} WifiMessageType;

/**
 * Status Codes.
 */
typedef enum {
  WIFI_STATUS_OK = 0,
  WIFI_STATUS_ERROR = 1
} WifiStatusCode;

/*
 * Notification Types.
 */
typedef enum {
  WIFI_NOTIFICATION_EVENT
} WifiNotificationType;

#ifdef __cplusplus
extern "C" {
#endif

struct WifiMsgHeader {
  uint16_t msgCategory;
  uint16_t msgType;
  uint32_t len;
} __attribute__((packed));

struct WifiMsg {
  struct WifiMsgHeader hdr;
  uint8_t data[];
} __attribute__((packed));

struct WifiMsgReq {
  struct WifiMsgHeader hdr;
  uint16_t sessionId;
  uint8_t data[];
} __attribute__((packed));

struct WifiMsgResp {
  struct WifiMsgHeader hdr;
  uint16_t sessionId;
  uint16_t status;
  uint8_t data[];
} __attribute__((packed));

struct WifiMsgNotify {
  struct WifiMsgHeader hdr;
  uint8_t data[];
} __attribute__((packed));

// WiFi message data
struct WifiMsgVersion {
  uint16_t majorVersion;
  uint16_t minorVersion;
} __attribute__((packed));

struct WifiMsgStartStopSupp {
  bool isP2pSupported;
} __attribute__((packed));

#ifdef __cplusplus
}
#endif

struct WifiMsgNotifyEvent {
  char eventMsg[];
};

class WifiBaseMessage
{
public:
  virtual ~WifiBaseMessage() {};

  virtual uint8_t* GetBuffer() = 0;

  virtual size_t GetLength() = 0;
};

template<typename T>
class WifiEmptyMessage : public WifiBaseMessage
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

  uint8_t* GetBuffer()
  {
    assert(mData);

    return mData;
  }

  size_t GetLength()
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
class WifiMessage : public WifiBaseMessage
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

  uint8_t* GetBuffer()
  {
    assert(mData);

    return mData;
  }

  size_t GetLength()
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

  uint16_t GetSessionId()
  {
    return WifiMessage<struct WifiMsgReq, T>::mMsg->aSessionId;
  }

  void SetSessionId(uint16_t aSessionId)
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

  uint16_t GetSessionId()
  {
    return WifiMessage<struct WifiMsgResp, T>::mMsg->aSessionId;
  }

  void SetSessionId(uint16_t aSessionId)
  {
    WifiMessage<struct WifiMsgResp, T>::mMsg->sessionId = aSessionId;
  }

  uint16_t GetStatus()
  {
    return WifiMessage<struct WifiMsgResp, T>::mMsg->status;
  }

  void SetStatus(WifiStatusCode aStatus)
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

} //namespace wifi

#endif // WifiGonkMessage_h
