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

#include <pthread.h>
#include <stdlib.h>
#include <cutils/properties.h>

#include "Controller.h"
#include "MessageHandlerListener.h"
#include "wifi.h"
#include "WpaSupplicantController.h"

#define P2P_SUPPORT_PROPERTY "ro.moz.wifi.p2p_supported"
#define BUFFER_SIZE        4096

#define CONTROLLER_LOAD_DRIVER() wifi_load_driver()
#define CONTROLLER_UNLOAD_DRIVER() wifi_unload_driver()
#define CONTROLLER_START_WPA_SUPPLICANT(p2p) wifi_start_supplicant(p2p)
#define CONTROLLER_STOP_WPA_SUPPLICANT(p2p) wifi_stop_supplicant(p2p)
#define CONTROLLER_CONNECT_TO_WPA_SUPPLICANT(interface) \
  wifi_connect_to_supplicant(interface)
#define CONTROLLER_CLOSE_WPA_SUPPLICANT_CONNECTION(interface) \
  wifi_close_supplicant_connection(interface)
#define CONTROLLER_COMMAND(interface, cmd, reply, replyLen) \
  wifi_command(interface, cmd, reply, replyLen)

using namespace wifi;

namespace wifi {
namespace controller {

const size_t EVENT_BUF_SIZE = 2048;

static pthread_t sThread;

static void* StaticThreadFunc(void* aArg)
{
  pthread_setname_np(pthread_self(), "WpaSupplicantController thread");
  WpaSupplicantController* controller =
    reinterpret_cast<WpaSupplicantController*>(aArg);
  return controller->WaitForEvent();
}

int WpaSupplicantController::GetWifiP2pSupported()
{
  char propP2pSupported[PROPERTY_VALUE_MAX];
  property_get(P2P_SUPPORT_PROPERTY, propP2pSupported, "0");
  return (0 == strcmp(propP2pSupported, "1") ? 1 : 0);
}

WpaSupplicantController::WpaSupplicantController(MessageHandlerListener* aListener)
  : Controller(aListener)
{
}

WpaSupplicantController::~WpaSupplicantController()
{
}

void WpaSupplicantController::StartMonitorEvent()
{
  mMonitored = true;
  if (pthread_create(&sThread, NULL, StaticThreadFunc, this) != 0) {
    abort();
  }
}

void WpaSupplicantController::StopMonitorEvent()
{
  mMonitored = false;
}

int WpaSupplicantController::HandleRequest(uint16_t aType,
                                           uint8_t* aData,
                                           size_t aDataLen) {
  assert(aData);

  int ret = 0;
  size_t len = BUFFER_SIZE - 1;
  char buffer[BUFFER_SIZE];

  switch (aType) {
    case WIFI_MESSAGE_TYPE_LOAD_DRIVER:
      ret = CONTROLLER_LOAD_DRIVER();
      break;

    case WIFI_MESSAGE_TYPE_UNLOAD_DRIVER:
      ret = CONTROLLER_UNLOAD_DRIVER();
      break;

    case WIFI_MESSAGE_TYPE_START_SUPPLICANT:
      ret = CONTROLLER_START_WPA_SUPPLICANT(GetWifiP2pSupported());
      break;

    case WIFI_MESSAGE_TYPE_STOP_SUPPLICANT:
      ret = CONTROLLER_STOP_WPA_SUPPLICANT(GetWifiP2pSupported());
      StopMonitorEvent();
      break;

    case WIFI_MESSAGE_TYPE_CONNECT_TO_SUPPLICANT:
      //TODO
      break;

    case WIFI_MESSAGE_TYPE_CLOSE_SUPPLICANT_CONNECTION:
      //TODO
      break;

    case WIFI_MESSAGE_TYPE_COMMAND:
      //TODO
      break;

    default:
      break;
  }

  mListener->OnResponse(static_cast<WifiMessageType>(aType),
                        static_cast<WifiStatusCode>(ret ? 1 : 0),
                        static_cast<void*>(buffer),
                        len);
  return 0;

}

void* WpaSupplicantController::WaitForEvent() {
  while(mMonitored) {
    char buf[EVENT_BUF_SIZE];
    int nread = wifi_wait_for_event(buf, sizeof buf);
    if (nread <= 0) {
      continue;
    }

    //dispatch event

    //TODO we handle stop monitor in gecko.
    //Do we need handle anything here?
  }
  return NULL;

}
} //controller
} //wifi
