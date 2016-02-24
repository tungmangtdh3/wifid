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

#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "Controller.h"
#include "HostApdController.h"
#include "libwpa_client/wpa_ctrl.h"
#include "MessageHandlerListener.h"
#include "WifiDebug.h"

#define BUFFER_SIZE        4096

namespace wifi {
namespace controller {

static const char HOSTAPD_NAME[] = "hostapd";
static const char HOSTAPD_CTRL_DIR[] = "/data/misc/wifi/hostapd";

static struct wpa_ctrl *sCtrlConn;

char* HostApdController::GetWifiIfname()
{
  char *ifname = NULL;
  struct dirent *dent;

  DIR *dir = opendir(HOSTAPD_CTRL_DIR);
  if (dir) {
    while ((dent = readdir(dir))) {
      if (strcmp(dent->d_name, ".") == 0 ||
          strcmp(dent->d_name, "..") == 0) {
        continue;
      }
      ifname = strdup(dent->d_name);
      break;
    }
    closedir(dir);
  }
  return ifname;
}

int32_t HostApdController::ConnectHostApd()
{
  char ctrlConnPath[256];
  int32_t retryTimes = 20;

  if(*GetWifiIfname() == '\0') {
    WIFID_DEBUG("wifi_connect_to_hostapd: GetWifiIfname fail");
    return -1;
  }

  snprintf(ctrlConnPath, sizeof(ctrlConnPath), "%s/%s", HOSTAPD_CTRL_DIR, GetWifiIfname());
  WIFID_DEBUG("ctrlConnPath = %s\n", ctrlConnPath);

  { /* check iface file is ready */
    int cnt = 100;
    sched_yield();
    while (access(ctrlConnPath, F_OK | W_OK)!=0 && cnt-- > 0) {
      usleep(1000);
    }
    if (access(ctrlConnPath, F_OK | W_OK)==0) {
      WIFID_DEBUG("ctrlConnPath %s is ready to read/write cnt=%d\n", ctrlConnPath, cnt);
    } else {
      WIFID_DEBUG("ctrlConnPath %s is not ready, cnt=%d\n", ctrlConnPath, cnt);
    }
  }

  while (retryTimes--){
    sCtrlConn = wpa_ctrl_open(ctrlConnPath);
    if (NULL == sCtrlConn) {
      usleep(1000);
      WIFID_DEBUG("Retry to wpa_ctrl_open \n");
    } else {
      break;
    }
  }

  if (NULL == sCtrlConn) {
    WIFID_ERROR("Unable to open connection to supplicant on \"%s\": %s",
        ctrlConnPath, strerror(errno));
    return -1;
  }

  if (wpa_ctrl_attach(sCtrlConn) != 0) {
    wpa_ctrl_close(sCtrlConn);
    sCtrlConn = NULL;
    return -1;
  }
  return 0;
}

int32_t HostApdController::CloseHostApd()
{
  if (!sCtrlConn) {
    return -1;
  }

  if (wpa_ctrl_detach(sCtrlConn) < 0) {
    WIFID_WARNING("Failed to detach wpa_ctrl.");
  }

  if (sCtrlConn != NULL) {
    wpa_ctrl_close(sCtrlConn);
    sCtrlConn = NULL;
  }
  return 0;

}

HostApdController::HostApdController(MessageHandlerListener* aListener)
  : Controller(aListener)
{
}

HostApdController::~HostApdController() {
}

int32_t HostApdController::SendCommand(const char *aCmd,
                                       char *aReply,
                                       size_t *aReplyLen) {
  int32_t ret;

  if (!sCtrlConn) {
    WIFID_WARNING("Not connected to hostapd - \"%s\" command dropped.\n", aCmd);
    return -1;
  }

  ret = wpa_ctrl_request(sCtrlConn, aCmd, strlen(aCmd), aReply, aReplyLen, NULL);

  if (ret == -2) {
    WIFID_WARNING("'%s' command timed out.\n", aCmd);
    return -2;
  } else if (ret < 0 || strncmp(aReply, "FAIL", 4) == 0) {
    return -1;
  }

  // Make the reply printable.
  aReply[*aReplyLen] = '\0';
  if (strncmp(aCmd, "STA-FIRST", 9) == 0 ||
      strncmp(aCmd, "STA-NEXT", 8) == 0) {
    char *pos = aReply;

    while (*pos && *pos != '\n')
      pos++;
    *pos = '\0';
  }

  return 0;
}
int32_t HostApdController::HandleRequest(uint16_t aType,
                                     uint8_t* aData,
                                     size_t aDataLen) {
  assert(aData);

  int32_t ret = 0;
  size_t len = BUFFER_SIZE - 1;
  char buffer[BUFFER_SIZE];

  switch (aType) {
    case WIFI_MESSAGE_TYPE_CONNECT_TO_HOSTAPD:
      ret = ConnectHostApd();
      break;

    case WIFI_MESSAGE_TYPE_CLOSE_HOSTAPD_CONNECTION:
      ret = CloseHostApd();
      break;

    case WIFI_MESSAGE_TYPE_HOSTAPD_GET_STATIONS:
      //TODO
      break;

    case WIFI_MESSAGE_TYPE_HOSTAPD_COMMAND:
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

int32_t HostApdController::GetStations() {
  char addr[32], cmd[64];
  int32_t stations = 0;
  size_t addrLen = sizeof(addr);

  if (!sCtrlConn) {
    WIFID_WARNING("Not connected to hostapd - \"%s\" command dropped.\n", cmd);
    return -1;
  }

  if (SendCommand("STA-FIRST", addr, &addrLen)) {
    return 0;
  }
  stations++;

  snprintf(cmd, sizeof(cmd), "STA-NEXT %s", addr);
  while (SendCommand(cmd, addr, &addrLen) == 0) {
    stations++;
    snprintf(cmd, sizeof(cmd), "STA-NEXT %s", addr);
  }

  return stations;
}
} //controller
} //wifi
