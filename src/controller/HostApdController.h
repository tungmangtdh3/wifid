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

#ifndef HostApdController_h
#define HostApdController_h

#include <linux/in.h>
#include <net/if.h>

#include "Controller.h"

namespace wifi {

class MessageHandlerListener;

namespace controller {

class HostApdController : public Controller {
public:
  HostApdController(MessageHandlerListener* aListener);

  ~HostApdController();

  int HandleRequest(uint16_t aType, uint8_t* aData, size_t aDataLen);

private:
  int32_t ConnectHostApd();

  int32_t CloseHostApd();

  int32_t GetStations();

  int32_t SendCommand(const char *aCmd,
                      char *aReply,
                      size_t *aReplyLen);

  char *GetWifiIfname();

  pid_t mPid;
};
} //controller
} //wifi
#endif
