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
 * File:   testCUdpSocket.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 23, 2021, 3:13 PM
 */

#include "testCUdpSocket.h"
#include "CSocketTestProxy.h"
#include "../CUdpSocket.h"
#include <fcntl.h>


CPPUNIT_TEST_SUITE_REGISTRATION(testCUdpSocket);


testCUdpSocket::testCUdpSocket()
{
}

testCUdpSocket::~testCUdpSocket()
{
}

void testCUdpSocket::setUp()
{
}

void testCUdpSocket::tearDown()
{
}

void testCUdpSocket::testCloseUdpSocket()
{
   CUdpSocket udpSocket;

   // if udpSocket is not opened then no socket is opened and OS will report an error
   CPPUNIT_ASSERT_THROW(udpSocket.closeUdpSocket(), std::runtime_error);

   CPPUNIT_ASSERT_NO_THROW(udpSocket.openUdpSocket());

   CPPUNIT_ASSERT(udpSocket.isOpen());

   CPPUNIT_ASSERT_NO_THROW(udpSocket.closeUdpSocket());

   CPPUNIT_ASSERT(udpSocket.isOpen()==false);
}

void testCUdpSocket::testOpenUdpSocket()
{
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);
   CUdpSocket udpSocket;

   udpSocket.setSocketProxy(testProxy);

   CPPUNIT_ASSERT_NO_THROW(udpSocket.openUdpSocket());
   CPPUNIT_ASSERT(udpSocket.isOpen());
   CPPUNIT_ASSERT_EQUAL(0, testProxy->closeCnt);

   CPPUNIT_ASSERT_NO_THROW(udpSocket.openUdpSocket());
   CPPUNIT_ASSERT(udpSocket.isOpen());
   // verify that old socket is closed
   CPPUNIT_ASSERT_EQUAL(1, testProxy->closeCnt);

   // define a test proxy to simulate that socket system call fails
   class CSocketTestProxySocketError : public CSocketTestProxy
   {
   public:
      virtual int socket(int socket_family, int socket_type, int protocol) override
      {
         Errno = EACCES;   // simulate no permission error
         return -1;
      }
   };

   testProxy.reset(new CSocketTestProxySocketError);
   udpSocket.setSocketProxy(testProxy);

   try
   {
      udpSocket.openUdpSocket();
      CPPUNIT_FAIL("Error, we expect openUdpSocket() to throw a runtime_error");
   }
   catch (std::runtime_error &re)
   {
      testProxy->verifyErrnoInMessage(re.what());
      CPPUNIT_ASSERT(udpSocket.isOpen() == false);
   }
}

void testCUdpSocket::testSetNonBlocking()
{
   CUdpSocket udpSocket;

   // if udpSocket is not opened then no socket is opened and OS will report an error
   CPPUNIT_ASSERT_THROW(udpSocket.setNonBlocking(), std::runtime_error);

   udpSocket.openUdpSocket();
   // a normally opened socket must have the ability to be set to non blocking
   CPPUNIT_ASSERT_NO_THROW(udpSocket.setNonBlocking());

   // create a test proxy to force an error on the second call of fcntl
   class CTestProxyFcntlError : public CSocketTestProxy
   {
   public:
      virtual int fcntl(int fd, int cmd, int param) override
      {
         if (cmd == F_SETFL)
         {
            fcntlCnt++; Errno = ENOMEM;
            return -1;
         }
         return  CSocketTestProxy::fcntl(fd, cmd, param);
      }
   };

   std::shared_ptr<CTestProxyFcntlError> testProxy( new CTestProxyFcntlError );
   udpSocket.setSocketProxy(testProxy);

   try
   {
      udpSocket.setNonBlocking();
      CPPUNIT_FAIL("Error, we expect setNonBlocking() to throw a runtime_error");
   }
   catch (std::runtime_error &re)
   {
      testProxy->verifyErrnoInMessage(re.what());
      CPPUNIT_ASSERT_EQUAL(2, testProxy->fcntlCnt);
   }
}
