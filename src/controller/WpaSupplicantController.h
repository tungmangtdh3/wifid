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

#ifndef WpaSupplicantController_h
#define WpaSupplicantController_h

#include "Controller.h"

namespace wifi {
namespace controller {

class WpaSupplicantController : public Controller {
public:
  WpaSupplicantController(MessageHandlerListener* aListener);

  ~WpaSupplicantController();

  int32_t HandleRequest(uint16_t aType, uint8_t* aData, size_t aDataLen);

  void* WaitForEvent();

private:
  void StartMonitorEvent();

  void StopMonitorEvent();

  int32_t GetWifiP2pSupported();

  bool mMonitored;
};
} //controller
} //wifi
#endif //WpaSupplicantController_h
