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

#ifndef CommandLine_h
#define CommandLine_h

/// Command line options
typedef struct {
  bool useListenSocket;
  const char* socketName;
  bool useSeqPacket;
} CommandLineOptions;

int32_t ParseCommandLine(int32_t aArgc, char* aArgv[], CommandLineOptions *op);

#endif
