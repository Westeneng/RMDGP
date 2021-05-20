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
#include "../CSocketAddress.h"
#include "../CTime.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


CPPUNIT_TEST_SUITE_REGISTRATION(testCUdpSocket);

namespace 
{
   const char testMessage[] = "This is a test message :-)";
}

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

   // create a test proxy to simulate an error on the second call of fcntl
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

void testCUdpSocket::testSendTo()
{
   const struct in_addr localAddress = { inet_addr("127.0.0.1") };
   CSocketAddress destination("127.0.0.1", 7777); // encapsulates sockaddr_in
   sockaddr_in sourceAddress = {0};
   CTime waitTime(0,1000);
   CUdpSocket udpSocket;
   CUdpSocket udpReceiver;

   CPPUNIT_ASSERT_NO_THROW(udpReceiver.openUdpSocket());
   CPPUNIT_ASSERT_NO_THROW(udpReceiver.bind(localAddress, 7777));
   CPPUNIT_ASSERT_NO_THROW(udpReceiver.setNonBlocking());

   CPPUNIT_ASSERT_NO_THROW(udpSocket.openUdpSocket());
   CPPUNIT_ASSERT_NO_THROW(udpSocket.bind(localAddress, 7000));
   CPPUNIT_ASSERT_NO_THROW(udpSocket.sendTo(testMessage, sizeof(testMessage), &destination));

   nanosleep(&waitTime, NULL); // give upd/ip stack some time

   char buffer[1024] = {0};
   size_t len;
   CPPUNIT_ASSERT_NO_THROW(len = udpReceiver.receiveFrom(buffer, sizeof(buffer), &sourceAddress));
   CPPUNIT_ASSERT_EQUAL(std::string(testMessage), std::string(buffer));
   CPPUNIT_ASSERT_EQUAL(sizeof(testMessage), len);

   // extra to also cover receive from.
   CPPUNIT_ASSERT_EQUAL(localAddress.s_addr, sourceAddress.sin_addr.s_addr);
   CPPUNIT_ASSERT_EQUAL(uint16_t(7000), ntohs(sourceAddress.sin_port));
   CPPUNIT_ASSERT_EQUAL(sa_family_t(AF_INET), sourceAddress.sin_family);
}

void testCUdpSocket::testSendToThrow()
{
   CUdpSocket udpSocket;
   CSocketAddress destination("127.0.0.1", 7777); // encapsulates sockaddr_in

   // if udpSocket is not opened then no socket is opened and OS will report an error
   CPPUNIT_ASSERT_THROW(udpSocket.setNonBlocking(), std::runtime_error);
}

void testCUdpSocket::testSendToInterrupted()
{
   CUdpSocket udpSocket;
   CSocketAddress destination("127.0.0.1", 7777); // encapsulates sockaddr_in

   // define a test proxy to simulate that sendto system call reports Interrupted three times
   class CSocketTestProxySendToError : public CSocketTestProxy
   {
   public:
      virtual ssize_t sendto(int fd, const void *buf, size_t len, int flags,
                          const struct sockaddr *dest_addr, socklen_t addrlen) override
      {
         if(++sendtoCnt > 3)
         {
            Errno = 0;
            return CSocketProxy::sendto(fd, buf, len, flags, dest_addr, addrlen);
         }
         Errno = EINTR;
         return -1;
      }
   };
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxySendToError);
   udpSocket.setSocketProxy(testProxy);

   CPPUNIT_ASSERT_NO_THROW(udpSocket.openUdpSocket());
   CPPUNIT_ASSERT_EQUAL(sizeof(testMessage),
                        udpSocket.sendTo(testMessage, sizeof(testMessage), &destination));
   CPPUNIT_ASSERT_EQUAL(4, testProxy->sendtoCnt);
}

void testCUdpSocket::testSendToWouldBlock()
{
   CUdpSocket udpSocket;
   CSocketAddress destination("127.0.0.1", 7777); // encapsulates sockaddr_in

   // define a test proxy to simulate that sendto system call reports EWOULDBLOCK
   class CSocketTestProxySendToError : public CSocketTestProxy
   {
   public:
      virtual ssize_t sendto(int fd, const void *buf, size_t len, int flags,
                          const struct sockaddr *dest_addr, socklen_t addrlen) override
      {
         Errno = (sendtoCnt%2 == 0) ? EWOULDBLOCK : EAGAIN;
         sendtoCnt++; return -1;
      }
   };
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxySendToError);
   udpSocket.setSocketProxy(testProxy);

   CPPUNIT_ASSERT_NO_THROW(udpSocket.openUdpSocket());

   // EWOULDBLOCK
   CPPUNIT_ASSERT_EQUAL(size_t(0), udpSocket.sendTo(testMessage, sizeof(testMessage), &destination));
   // second time EAGAIN
   CPPUNIT_ASSERT_EQUAL(size_t(0), udpSocket.sendTo(testMessage, sizeof(testMessage), &destination));
}

void testCUdpSocket::testReceiveFromThrow()
{
   CUdpSocket udpSocket;
   sockaddr_in sourceAddress = {0};
   char buffer[1024] = {0};
   // if udpSocket is not opened then no socket is opened and OS will report an error
   CPPUNIT_ASSERT_THROW(udpSocket.receiveFrom(buffer, sizeof(buffer), &sourceAddress),
           std::runtime_error);
}

void testCUdpSocket::testReceiveFromInterrupted()
{
   const struct in_addr localAddress = { inet_addr("127.0.0.1") };
   CUdpSocket udpSocket;
   sockaddr_in sourceAddress = {0};
   char buffer[1024] = {0};

   // define a test proxy to simulate that recvfrom system call reports Interrupted three times
   class CSocketTestProxyRecvFromInterrupted : public CSocketTestProxy
   {
   public:
      virtual ssize_t recvfrom(int fd, void *buf, size_t len, int flags,
                      struct sockaddr *src_addr, socklen_t *addrlen) override
      {
         if(++recvfromCnt > 3)
         {
            Errno = 0;
            return CSocketProxy::recvfrom(fd, buf, len, flags, src_addr, addrlen);
         }
         Errno = EINTR;
         return -1;
      }
   };
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxyRecvFromInterrupted);
   udpSocket.setSocketProxy(testProxy);

   CPPUNIT_ASSERT_NO_THROW(udpSocket.openUdpSocket());
   CPPUNIT_ASSERT_NO_THROW(udpSocket.setNonBlocking());
   CPPUNIT_ASSERT_NO_THROW(udpSocket.bind(localAddress, 7000));

   // receiveFrom will return 0 because of non blocking mode.
   CPPUNIT_ASSERT_EQUAL(size_t(0), udpSocket.receiveFrom(buffer, sizeof(buffer), &sourceAddress));
   CPPUNIT_ASSERT_EQUAL(4, testProxy->recvfromCnt);
}

void testCUdpSocket::testReceiveFromWouldBlock()
{
   // tested at testReceiveFromInterrupted
   CPPUNIT_ASSERT(true);
}

void testCUdpSocket::testGetLocalSockAddress()
{
   //  tested at tstBindDataDriven
   CPPUNIT_ASSERT(true);
}
