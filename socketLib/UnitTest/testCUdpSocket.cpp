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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


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

void testCUdpSocket::testRetrieveInterfaceAdressFromAddress()
{
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);
   CUdpSocket udpSocket;
   const in_addr localAddress = { inet_addr("127.0.0.1") };
   const in_addr illegalAddress = { inet_addr("120.0.0.1") };
   const in_addr anyAddress = { INADDR_ANY };

   udpSocket.setSocketProxy(testProxy);

   in_addr result = udpSocket.retrieveInterfaceAdressFromAddress(localAddress);
   CPPUNIT_ASSERT_EQUAL(localAddress.s_addr, result.s_addr);
   CPPUNIT_ASSERT_EQUAL(1, testProxy->getifaddrsCnt);
   CPPUNIT_ASSERT_EQUAL(1, testProxy->freeifaddrsCnt);

   result = udpSocket.retrieveInterfaceAdressFromAddress(illegalAddress);
   CPPUNIT_ASSERT_EQUAL(anyAddress.s_addr, result.s_addr);
   CPPUNIT_ASSERT_EQUAL(2, testProxy->getifaddrsCnt);
   CPPUNIT_ASSERT_EQUAL(2, testProxy->freeifaddrsCnt);
}

void testCUdpSocket::testRetrieveInterfaceAdressFromAddressThrow()
{
   CUdpSocket udpSocket;
   const in_addr localAddress = { inet_addr("127.0.0.1") };

   // create a test proxy to force simulate an error on call of getifaddrs
   class CTestProxyGetifaddrsError : public CSocketTestProxy
   {
   public:
      virtual int getifaddrs(struct ifaddrs **ifap) override
      {
         getifaddrsCnt++; Errno = ENOMEM;
         return -1;
      }
   };

   std::shared_ptr<CTestProxyGetifaddrsError> testProxy( new CTestProxyGetifaddrsError );
   udpSocket.setSocketProxy(testProxy);

   try
   {
      in_addr result = udpSocket.retrieveInterfaceAdressFromAddress(localAddress);
      CPPUNIT_FAIL("Error, we expect retrieveInterfaceAdressFromAddress() to throw a runtime_error");
   }
   catch (std::runtime_error &re)
   {
      testProxy->verifyErrnoInMessage(re.what());
      CPPUNIT_ASSERT_EQUAL(1, testProxy->getifaddrsCnt);
      CPPUNIT_ASSERT_EQUAL(0, testProxy->freeifaddrsCnt);
   }
}

void testCUdpSocket::testCloseAndThrowRuntimeException()
{
   CUdpSocket udpSocket;

   // create a test proxy to return an error on the first call of getErrno
   class CTestProxyGetErrno : public CSocketTestProxy
   {
   public:
      virtual int getErrno() override
      {
         int ret = Errno ? Errno : CSocketProxy::getErrno();
         Errno = 0;
         return ret;
      }
   };
   std::shared_ptr<CSocketTestProxy> testProxy(new CTestProxyGetErrno);

   udpSocket.setSocketProxy(testProxy);
   testProxy->Errno = ENOMEM;

   try
   {
      udpSocket.closeAndThrowRuntimeException("What is the matter?");
      CPPUNIT_FAIL("Error, we expect closeAndThrowRuntimeException() to throw a runtime_error");
   }
   catch (std::runtime_error &re)
   {
      // verify that given ENOMEM is generated.
      testProxy->Errno = ENOMEM;
      testProxy->verifyErrnoInMessage(re.what(), "ENOMEM");
      // because there is no open socket, we also expect a EBADF error
      testProxy->Errno = EBADF;
      testProxy->verifyErrnoInMessage(re.what(), "EBADF");

      std::string message(re.what());
      CPPUNIT_ASSERT(message.find("What is the matter?") != std::string::npos);
   }
}

void testCUdpSocket::testBind()
{
   CUdpSocket udpSocket;
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);
   const struct in_addr localAddress = { inet_addr("127.0.0.1") };
   const struct in_addr anyAddress = { INADDR_ANY };
   const int port = 7000;

   tstBindDataDriven("local address port 0", localAddress, 0, false);
   tstBindDataDriven("local address port set", localAddress, port, true);
   tstBindDataDriven("any address port 0", localAddress, 0, false);
   tstBindDataDriven("any address port set", localAddress, port+1, true);
}

void testCUdpSocket::tstBindDataDriven( const std::string testName,
                        const struct in_addr &interfaceAddress,
                        int port,
                        bool equalPortExpected)
{
   CUdpSocket udpSocket;
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);

   udpSocket.setSocketProxy(testProxy);

   CPPUNIT_ASSERT_NO_THROW_MESSAGE(testName, udpSocket.openUdpSocket());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE(testName, udpSocket.bind(interfaceAddress, port));

   if(equalPortExpected)
   {
      CPPUNIT_ASSERT_EQUAL_MESSAGE(testName, port, udpSocket.getLocalPort());

      sockaddr_in sockAddress = udpSocket.getLocalSockAddress();
      CPPUNIT_ASSERT_EQUAL_MESSAGE(testName, int(ntohs(sockAddress.sin_port)), port);
   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE(testName, udpSocket.getLocalPort()>1000);
   }
   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName,
           interfaceAddress.s_addr, udpSocket.getLocalAddress().s_addr);
}

void testCUdpSocket::testBindException()
{
   CUdpSocket udpSocket;
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);
   const in_addr localAddress = { inet_addr("127.0.0.1") };
   const int invalidPort = 1;

   udpSocket.setSocketProxy(testProxy);

   CPPUNIT_ASSERT_NO_THROW(udpSocket.openUdpSocket());

   try
   {
      udpSocket.bind(localAddress, invalidPort);
      CPPUNIT_FAIL("Error, we expect bind() to throw a runtime_error");
   }
   catch (std::runtime_error &re)
   {
      testProxy->Errno = EACCES;
      testProxy->verifyErrnoInMessage(re.what());
      CPPUNIT_ASSERT(udpSocket.isOpen()==false);
   }
}

void testCUdpSocket::testGetLocalSockAddress()
{
   //  tested at tstBindDataDriven
   CPPUNIT_ASSERT(true);
}
