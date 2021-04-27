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
 * File:   testCUdpMulticastReceiver.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on Apr 14, 2021, 8:30:06 PM
 */

#ifndef TESTCUDPMULTICASTRECEIVER_H
#define TESTCUDPMULTICASTRECEIVER_H

#include <memory>
#include <cppunit/extensions/HelperMacros.h>
class CUdpMulticastReceiver;
class CSocketTestProxy;

class testCUdpMulticastReceiver : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testCUdpMulticastReceiver);

    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testDestructor);
    CPPUNIT_TEST(testOpen);
    CPPUNIT_TEST(testOpenThrow);
    CPPUNIT_TEST(testReceive);
    CPPUNIT_TEST(testReceiveThrow);
    CPPUNIT_TEST(testGetSourcePortNumber);

    CPPUNIT_TEST_SUITE_END();

public:
    testCUdpMulticastReceiver();
    virtual ~testCUdpMulticastReceiver();
    void setUp();
    void tearDown();

private:
    void testConstructor();
    void testCopyConstructor();
    void tstCopyConstructorDataDriven(const std::string testName, const CUdpMulticastReceiver &orig);
    void testDestructor();
    void testOpen();
    void testOpenThrow();
    void tstOpenThrowDataDriven(const std::string testName,
                                const std::string multicastAddress, int multicastPort,
                                const std::string sourceAddress, int sourcePort,
                                const std::string expectedText, int expectedErrno,
                                std::shared_ptr<CSocketTestProxy> testProxy);
    void testReceive();
    void testReceiveThrow();
    void testGetSourcePortNumber();
};

#endif /* TESTCUDPMULTICASTRECEIVER_H */

