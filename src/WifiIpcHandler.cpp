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

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "WifiDebug.h"
#include "WifiIpcHandler.h"

namespace wifi {

WifiIpcHandler::WifiIpcHandler(int aSockMode, const char* aSockName, bool aIsSeqPacket)
  : mRwFd(-1)
  , mConnFd(-1)
  , mSockMode(aSockMode)
  , mSockName(aSockName)
  , mIsSeqPacket(aIsSeqPacket)
  , mIsConnected(false)
{
}

WifiIpcHandler::~WifiIpcHandler()
{
  closeIpc();
}

int
WifiIpcHandler::openIpc()
{
  int ret = -1;

  if (mIsConnected) {
    return 0;
  }

  switch (mSockMode) {
    case CONNECT_MODE:
      ret = openConnectSocket();
      break;

    case LISTEN_MODE:
      ret = openListenSocket();
      break;

    default:
      WIFID_ERROR("Could not recognize the socket mode(%d).\n", mSockMode);
  }

  settingSocket();

  mIsConnected = true;

  return ret;
}

int
WifiIpcHandler::readIpc(uint8_t* aData, size_t aDataLen)
{
  if (!mIsConnected) {
    return -1;
  }

  return read(mRwFd, aData, aDataLen);
}

int
WifiIpcHandler::writeIpc(uint8_t* aData, size_t aDataLen)
{
  size_t writeOffset = 0;
  int size;

  if (!mIsConnected) {
    return -1;
  }

  if (!aData) {
    return -1;
  }

  while (writeOffset < aDataLen) {
    do {
      size = write(mRwFd, aData + writeOffset, aDataLen - writeOffset);
    } while (size < 0 && errno == EINTR);

    if (size < 0) {
      WIFID_ERROR("Response: unexpected error on write errno:%d", errno);
      return -1;
    }

    writeOffset += size;
  }

  return 0;
}

int
WifiIpcHandler::waitForData()
{
  struct pollfd fds[1];
  int ret;

  if (!mIsConnected) {
    return -1;
  }

  fds[0].fd = mRwFd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  do {
    ret = poll(fds, 1, -1);
  } while(ret < 0 && errno == EINTR);

  return ret;
}

int WifiIpcHandler::closeIpc()
{
  if (mRwFd != -1) {
    close(mRwFd);
  }

  if (mConnFd != -1) {
    close(mConnFd);
  }

  mIsConnected = false;

  return 0;
}

int
WifiIpcHandler::openConnectSocket()
{
  size_t len, siz;
  struct sockaddr_un addr;
  int res;

  len = strlen(mSockName);
  siz = len + NBOUNDS;
  if (siz > UNIX_PATH_MAX) {
    WIFID_ERROR("Socket address too long\n");
    return -1;
  }

  addr.sun_family = AF_UNIX;
  addr.sun_path[0] = '\0'; /* abstract socket namespace */
  memcpy(addr.sun_path + 1, mSockName, len + 1);
  socklen_t addrLen = offsetof(struct sockaddr_un, sun_path) + siz;

  mRwFd = socket(AF_UNIX, mIsSeqPacket ? SOCK_SEQPACKET : SOCK_STREAM, 0);
  if (mConnFd < 0) {
    WIFID_ERROR("Could not create %s socket: %s\n", mSockName, strerror(errno));
    return -1;
  }

  res = TEMP_FAILURE_RETRY(
    connect(mRwFd, reinterpret_cast<struct sockaddr*>(&addr), addrLen));
  if (res < 0) {
    WIFID_ERROR("Could not connect %s socket: %s\n", mSockName, strerror(errno));
    close(mRwFd);
    return -1;
  }

  return 0;
}

int
WifiIpcHandler::openListenSocket()
{
  struct sockaddr_un hostaddr, peeraddr;
  socklen_t socklen = sizeof(hostaddr);
  int ret;
  size_t len, siz;

  len = strlen(mSockName);
  if ((len + NBOUNDS) > UNIX_PATH_MAX) {
    WIFID_ERROR("Socket address too long\n");
    return -1;
  }

  mConnFd = socket(AF_UNIX, mIsSeqPacket ? SOCK_SEQPACKET : SOCK_STREAM, 0);

  if (mConnFd < 0) {
    WIFID_ERROR("Could not create %s socket: %s\n", mSockName, strerror(errno));
    return -1;
  }

  hostaddr.sun_family = AF_UNIX;
  hostaddr.sun_path[0] = '\0'; /* abstract socket namespace */
  memcpy(hostaddr.sun_path + 1, mSockName, len + 1);

  ret = bind(mConnFd, reinterpret_cast<struct sockaddr*>(&hostaddr), socklen);

  if (ret < 0) {
    WIFID_ERROR("Could not bind %s socket: %s\n", mSockName, strerror(errno));
    close(mConnFd);
    return -1;
  }

  ret = listen(mConnFd, 4);

  if (ret < 0) {
    WIFID_ERROR("Could not listen %s socket: %s\n", mSockName, strerror(errno));
    close(mConnFd);
    return -1;
  }

  mRwFd = accept(mConnFd, (struct sockaddr*)&peeraddr, &socklen);

  if (mRwFd < 0) {
    WIFID_ERROR("Error on accept(): %s\n", strerror(errno));
    close(mConnFd);
    return -1;
  }

  return 0;
}

bool
WifiIpcHandler::isConnected()
{
  return mIsConnected;
}

void
WifiIpcHandler::settingSocket()
{
  int ret = fcntl(mConnFd, F_SETFL, O_NONBLOCK);
  if (ret < 0) {
    WIFID_ERROR("Error setting O_NONBLOCK errno: %s\n", strerror(errno));
  }
}
}//namespace wifi
