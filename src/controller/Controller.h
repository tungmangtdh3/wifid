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

#ifndef Controller_h
#define Controller_h

#include "WifiMessage.h"

using namespace wifi;

namespace wifi {
class MessageHandlerListener;
}

namespace wifi {
namespace controller {

class Controller
{
public:
  Controller(MessageHandlerListener* aListener);
  virtual ~Controller();

  virtual int32_t HandleRequest(uint16_t aType, uint8_t* aData, size_t aDataLen) = 0;

protected:
  MessageHandlerListener* mListener;
};

} //controller
} //namespace wifi
#endif // Controller_h
