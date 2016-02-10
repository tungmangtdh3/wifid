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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

struct WifiMsgNotifyEvent {
  char eventMsg[];
};

#ifdef __cplusplus
}
#endif

#endif // WifiGonkMessage_h