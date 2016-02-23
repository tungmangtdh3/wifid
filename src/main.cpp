/*
 * Copyright (C) 2014-2015  Mozilla Foundation
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

#include <stdlib.h>

#include "Daemon.h"
#include "CommandLine.h"

static CommandLineOptions options;

int main(int argc, char* argv[]) {
  int res = ParseCommandLine(argc, argv, &options);

  if (res > 0) {
    return EXIT_SUCCESS;
  } else if (res < 0) {
    return EXIT_FAILURE;
  }

  if (!wifi::Daemon::Initialize(&options)) {
    return EXIT_FAILURE;
  }

  // Start the main loop.
  wifi::Daemon::GetInstance()->Start();

  // The main message loop has exited; clean up the Daemon.
  wifi::Daemon::GetInstance()->ShutDown();

  return EXIT_SUCCESS;
}
