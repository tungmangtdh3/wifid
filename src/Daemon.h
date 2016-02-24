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

#ifndef daemon_h
#define daemon_h

#include "CommandLine.h"

namespace wifi {

class Daemon {
 public:
  static bool Initialize(const CommandLineOptions *aOp);

  static void ShutDown();

  static Daemon& GetInstance();

  // Starts the daemon's main loop.
  virtual void Start() = 0;

 protected:
  Daemon() {};
  virtual ~Daemon();
 private:
  // Internal instance helper called by Initialize().
  virtual bool Init(const CommandLineOptions *aOp) = 0;

  Daemon(Daemon const&);
  void operator=(Daemon const&);
};
}
#endif // daemon_h
