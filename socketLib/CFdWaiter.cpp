/*
 * Copyright (C) 2021 G.J. Westeneng (Gerald Westeneng)
 *
 * This file is part of RMDGP. Reliable Multicast DataGram Protocol
 *
 * RMDGP is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * RMDGP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RMDGP.   If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * File:   CFdWaiter.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 30, 2021, 5:46 PM
 */

#include "CFdWaiter.h"
#include "CFileDescriptor.h"
#include "CClock.h"
#include "CSocketProxy.h"
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <sstream>



CFdWaiter::CFdWaiter() : proxy(CSocketProxySingleton::get())
{
}

CFdWaiter::~CFdWaiter()
{
}

int CFdWaiter::select(const CTime &timeout)
{
   int nfds = 0;
   CTime timeOut(timeout);

   FD_ZERO(&readFdSet);
   for(auto it = readFileDescriptors.begin(); it != readFileDescriptors.end(); ++it)
   {
      if((*it)->isOpen())
      {
         const int fd = (*it)->getFd();

         FD_SET(fd, &readFdSet);
         if(nfds <= fd)
            nfds = fd + 1;
      }
   }

   FD_ZERO(&writeFdSet);
   for(auto it = writeFileDescriptors.begin(); it != writeFileDescriptors.end(); ++it)
   {
      if((*it)->isOpen())
      {
         const int fd = (*it)->getFd();

         FD_SET(fd, &writeFdSet);
         if(nfds <= fd)
            nfds = fd + 1;
      }
   }

   return proxy->pselect(nfds, &readFdSet, &writeFdSet, NULL, &timeOut, NULL);
}

bool CFdWaiter::waitUntil(const CTime &moment)
{
   const CTime zeroTime(0,1);
   CTime currentTime;
   CTime timeout = moment - CClock::getMonotonicTime(currentTime);
   int result;

   while(timeout > zeroTime)
   {
      result = select(timeout);

      if(result==-1)
      {
         const int errorNumber = proxy->getErrno();
         if(errorNumber!=EINTR)
         {
            std::ostringstream message;
            message << "pselect Error " << errorNumber << ": " << strerror(errno);
            throw std::runtime_error(message.str());
         }
      }

      if(result>0)
         return false;

      timeout = moment - CClock::getMonotonicTime(currentTime);
   }
   return true;
}

void CFdWaiter::addReadFileDescriptor(const CFileDescriptor *fileDesriptor)
{
   readFileDescriptors.insert(fileDesriptor);
}

void CFdWaiter::delReadFileDescriptor(const CFileDescriptor *fileDesriptor)
{
   readFileDescriptors.erase(fileDesriptor);
}

void CFdWaiter::addWriteFileDescriptor(const CFileDescriptor *fileDesriptor)
{
   writeFileDescriptors.insert(fileDesriptor);
}

void CFdWaiter::delWriteFileDescriptor(const CFileDescriptor *fileDesriptor)
{
   writeFileDescriptors.erase(fileDesriptor);
}

void CFdWaiter::setSocketProxy(std::shared_ptr<CSocketProxy> sockProxy)
{
   proxy = sockProxy;
}