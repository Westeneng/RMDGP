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
 * File:   testCFdWaiter.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 1, 2021, 9:20 AM
 */

#include "testCFdWaiter.h"
#include "CSocketTestProxy.h"
#include "../CFdWaiter.h"
#include "../CTime.h"
#include "../CClock.h"
#include "../CUdpSocket.h"
#include "../CSocketAddress.h"
#include <netinet/in.h>
#include <arpa/inet.h>


CPPUNIT_TEST_SUITE_REGISTRATION(testCFdWaiter);

namespace {
   const CTime milliSecond(0, CTime::nsecInMillisec);
}


testCFdWaiter::testCFdWaiter()
{
}

testCFdWaiter::~testCFdWaiter()
{
}

void testCFdWaiter::setUp()
{
}

void testCFdWaiter::tearDown()
{
}

void testCFdWaiter::testAddReadFileDescriptor()
{
   CFdWaiter fdWaiter;
   CUdpSocket udpSock, udpSock2;
   
   CPPUNIT_ASSERT_EQUAL(size_t(0), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.addReadFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(1), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.addReadFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(1), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.addReadFileDescriptor(&udpSock2);
   CPPUNIT_ASSERT_EQUAL(size_t(2), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.addReadFileDescriptor(&udpSock2);
   CPPUNIT_ASSERT_EQUAL(size_t(2), fdWaiter.getNumberReadFileDescriptors());
}

void testCFdWaiter::testDelReadFileDescriptor()
{
   CFdWaiter fdWaiter;
   CUdpSocket udpSock, udpSock2, udpSock3;

   CPPUNIT_ASSERT_EQUAL(size_t(0), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.delReadFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(0), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.addReadFileDescriptor(&udpSock);
   fdWaiter.addReadFileDescriptor(&udpSock2);
   CPPUNIT_ASSERT_EQUAL(size_t(2), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.delReadFileDescriptor(&udpSock3);
   CPPUNIT_ASSERT_EQUAL(size_t(2), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.delReadFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(1), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.delReadFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(1), fdWaiter.getNumberReadFileDescriptors());
   fdWaiter.delReadFileDescriptor(&udpSock2);
   CPPUNIT_ASSERT_EQUAL(size_t(0), fdWaiter.getNumberReadFileDescriptors());
}

void testCFdWaiter::testAddWriteFileDescriptor()
{
   CFdWaiter fdWaiter;
   CUdpSocket udpSock, udpSock2;
   
   CPPUNIT_ASSERT_EQUAL(size_t(0), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.addWriteFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(1), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.addWriteFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(1), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.addWriteFileDescriptor(&udpSock2);
   CPPUNIT_ASSERT_EQUAL(size_t(2), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.addWriteFileDescriptor(&udpSock2);
   CPPUNIT_ASSERT_EQUAL(size_t(2), fdWaiter.getNumberWriteFileDescriptors());   
}

void testCFdWaiter::testDelWriteFileDescriptor()
{
   CFdWaiter fdWaiter;
   CUdpSocket udpSock, udpSock2, udpSock3;

   CPPUNIT_ASSERT_EQUAL(size_t(0), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.delWriteFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(0), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.addWriteFileDescriptor(&udpSock);
   fdWaiter.addWriteFileDescriptor(&udpSock2);
   CPPUNIT_ASSERT_EQUAL(size_t(2), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.delWriteFileDescriptor(&udpSock3);
   CPPUNIT_ASSERT_EQUAL(size_t(2), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.delWriteFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(1), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.delWriteFileDescriptor(&udpSock);
   CPPUNIT_ASSERT_EQUAL(size_t(1), fdWaiter.getNumberWriteFileDescriptors());
   fdWaiter.delWriteFileDescriptor(&udpSock2);
   CPPUNIT_ASSERT_EQUAL(size_t(0), fdWaiter.getNumberWriteFileDescriptors());   
}

void testCFdWaiter::testWaitUntil()
{
   const struct in_addr localAddress = { inet_addr("127.0.0.1") };
   CFdWaiter fdWaiter;
   CUdpSocket udpReceiver, udpSender;
   CTime currentTime;

   CTime targetTime = CClock::getMonotonicTime(currentTime) + milliSecond;
   
   CPPUNIT_ASSERT_EQUAL(true, fdWaiter.waitUntil(targetTime));
   CPPUNIT_ASSERT_GREATER(targetTime, CClock::getMonotonicTime(currentTime));
   
   fdWaiter.addReadFileDescriptor(&udpReceiver);
   targetTime = CClock::getMonotonicTime(currentTime) + milliSecond;
   // socket not opened so timeout expected
   CPPUNIT_ASSERT_EQUAL(true, fdWaiter.waitUntil(targetTime));
   CPPUNIT_ASSERT_GREATER(targetTime, CClock::getMonotonicTime(currentTime));
   
   CPPUNIT_ASSERT_NO_THROW(udpReceiver.openUdpSocket());
   CPPUNIT_ASSERT_NO_THROW(udpReceiver.bind(localAddress, 7000));
   targetTime = CClock::getMonotonicTime(currentTime) + milliSecond;
   // socket not readable so timeout expected
   CPPUNIT_ASSERT_EQUAL(true, fdWaiter.waitUntil(targetTime));
   CPPUNIT_ASSERT_GREATER(targetTime, CClock::getMonotonicTime(currentTime));

   fdWaiter.addWriteFileDescriptor(&udpSender);
   targetTime = CClock::getMonotonicTime(currentTime) + milliSecond;
   // socket not opened so timeout expected
   CPPUNIT_ASSERT_EQUAL(true, fdWaiter.waitUntil(targetTime));
   CPPUNIT_ASSERT_GREATER(targetTime, CClock::getMonotonicTime(currentTime));

   CPPUNIT_ASSERT_NO_THROW(udpSender.openUdpSocket());
   targetTime = CClock::getMonotonicTime(currentTime) + milliSecond;
   // socket opened so NO timeout expected
   CPPUNIT_ASSERT_EQUAL(false, fdWaiter.waitUntil(targetTime));
   CPPUNIT_ASSERT_LESS(targetTime, CClock::getMonotonicTime(currentTime));

   fdWaiter.delWriteFileDescriptor(&udpSender);
   targetTime = CClock::getMonotonicTime(currentTime) + milliSecond;
   // udpSender deleted from fdWaiter so timeout expected
   CPPUNIT_ASSERT_EQUAL(true, fdWaiter.waitUntil(targetTime));
   CPPUNIT_ASSERT_GREATER(targetTime, CClock::getMonotonicTime(currentTime));
   
   CSocketAddress sockAddress("127.0.0.1", 7000);
   const char testMessage[] = "Helloooo . . .";
   CPPUNIT_ASSERT_EQUAL(sizeof(testMessage), 
           udpSender.sendTo(testMessage, sizeof(testMessage), &sockAddress));

   targetTime = CClock::getMonotonicTime(currentTime) + milliSecond;
   // socket readable so NO timeout expected
   CPPUNIT_ASSERT_EQUAL(false, fdWaiter.waitUntil(targetTime));
   CPPUNIT_ASSERT_LESS(targetTime, CClock::getMonotonicTime(currentTime));
}

void testCFdWaiter::testWaitUntilInterrupted()
{
   CFdWaiter fdWaiter;
   CTime currentTime;

   // create a test proxy to simulate an interrupted on call of pselect
   class CTestProxyPselectInterrupted : public CSocketTestProxy
   {
   public:
      virtual int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                          const struct timespec *timeout, const sigset_t *sigmask) override
      {
         if(++pselectCnt>5)
         {
            Errno = 0;
            return CSocketProxy::pselect(nfds, readfds, writefds, exceptfds, timeout, sigmask);
         }
         Errno = EINTR;
         return -1;
      }
   };
   std::shared_ptr<CTestProxyPselectInterrupted> testProxy( new CTestProxyPselectInterrupted );
   fdWaiter.setSocketProxy(testProxy);

   CTime targetTime = CClock::getMonotonicTime(currentTime) + milliSecond;
   /// no socket added so timeout expected
   CPPUNIT_ASSERT_EQUAL(true, fdWaiter.waitUntil(targetTime));
   CPPUNIT_ASSERT_GREATER(targetTime, CClock::getMonotonicTime(currentTime));
   // we expect 6 calls of pselect
   CPPUNIT_ASSERT_EQUAL(6, testProxy->pselectCnt);
}

void testCFdWaiter::testWaitUntilThrows()
{
   CFdWaiter fdWaiter;
   CTime currentTime;

   // create a test proxy to simulate an error on call of pselect
   class CTestProxyPselectError : public CSocketTestProxy
   {
   public:
      virtual int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                          const struct timespec *timeout, const sigset_t *sigmask) override
      {
         pselectCnt++; Errno = ENOMEM;
         return -1;
      }
   };
   std::shared_ptr<CTestProxyPselectError> testProxy( new CTestProxyPselectError );
   fdWaiter.setSocketProxy(testProxy);

   CTime targetTime = CClock::getMonotonicTime(currentTime) + milliSecond;

   try
   {
      fdWaiter.waitUntil(targetTime);
      CPPUNIT_FAIL("Error, we expect waitUntil() to throw a runtime_error");
   }
   catch(std::runtime_error &re)
   {
      testProxy->verifyErrnoInMessage(re.what());
   }
}
