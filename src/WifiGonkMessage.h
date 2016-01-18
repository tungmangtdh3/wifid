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

#ifndef WifiGonkMessage_h
#define WifiGonkMessage_h

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
 *   2 bytes of error code.
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
 * Error codes.
 */
typedef enum {
  WIFI_SUCCESS = 0,
  WIFI_ERROR_TIMEOUT = 1,
} WifiErrorCode;

/**
 * Request Types.
 */
typedef enum {
  WIFI_REQUEST_LOAD_DRIVER,
  WIFI_REQUEST_UNLOAD_DRIVER,
  WIFI_REQUEST_START_SUPPLICANT,
  WIFI_REQUEST_STOP_SUPPLICANT,
  WIFI_REQUEST_CONNECT_TO_SUPPLICANT,
  WIFI_REQUEST_CLOSE_SUPPLICANT_CONNECTION,
  WIFI_REQUEST_COMMAND,
} WifiRequestType;

/**
 * Response Types.
 */
typedef enum {
  WIFI_RESPONSE_LOAD_DRIVER,
  WIFI_RESPONSE_UNLOAD_DRIVER,
  WIFI_RESPONSE_START_SUPPLICANT,
  WIFI_RESPONSE_STOP_SUPPLICANT,
  WIFI_RESPONSE_CONNECT_TO_SUPPLICANT,
  WIFI_RESPONSE_CLOSE_SUPPLICANT_CONNECTION,
  WIFI_RESPONSE_COMMAND,
} WifiResponseType;

/*
 * Notification Types.
 */
typedef enum {
  WIFI_NOTIFICATION_EVENT;
} WifiNotificationType;

#ifdef __cplusplus
}
#endif

#endif // WifiGonkMessage_h