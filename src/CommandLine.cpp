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

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

#include "CommandLine.h"

static const char* DEFAULT_SOCKET_NAME = "wifid";

int32_t SetSocketName(char* aArg, CommandLineOptions *aOp)
{
  if (!aArg) {
    fprintf(stderr, "Error: No network address specified.");
    return -1;
  }
  if (!strlen(aArg)) {
    fprintf(stderr, "Error: The specified network address is empty.");
    return -1;
  }
  aOp->socketName = aArg;

  return 0;
}

int32_t ShowCommandUsage()
{
  printf("Usage: wifid [OPTION]\n"
      "\n"
      "General options:\n"
      "  -h    displays this help\n"
      "\n"
      "Networking:\n"
      "  -a    the network address\n"
      "  -S    use socket type SOCK_SEQPACKET\n"
      "  -L    use listen socket\n"
      "\n"
      "The only supported address family is AF_UNIX with abstract\n"
      "names.\n");

  return 1;
}

int32_t UseSeqSocket(CommandLineOptions *aOp)
{
  aOp->useSeqPacket = true;
  return 0;
}

int32_t UseListenSocket(CommandLineOptions *aOp)
{
  aOp->useListenSocket = true;
  return 0;
}

// Parse command line.
int32_t ParseCommandLine(int32_t aArgc, char* aArgv[], CommandLineOptions *aOp)
{
  char word[100];
  int32_t res = 0;

  opterr = 0; /* no default error messages from getopt */
  aOp->socketName = DEFAULT_SOCKET_NAME;
  aOp->useListenSocket = false;
  aOp->useSeqPacket = true;

  do {
    int32_t opt = getopt(aArgc, aArgv, "a:hS");

    if (opt < 0) {
      break; /* end of options */
    }
    switch (opt) {
      case 'a':
        res = SetSocketName(optarg, aOp);
        break;
      case 'h':
        res = ShowCommandUsage();
        break;
      case 'S':
        res = UseSeqSocket(aOp);
        break;
      case 'L':
        res = UseListenSocket(aOp);
        break;
      default:
        res = -1; /* unknown option */
        break;
    }
  } while (!res);

  return res;
}


