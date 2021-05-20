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
 * File:   testCInterfaces.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 20, 2021, 11:06 AM
 */

#ifndef TESTCINTERFACES_H
#define TESTCINTERFACES_H

#include <cppunit/extensions/HelperMacros.h>

class testCInterfaces : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testCInterfaces);

    CPPUNIT_TEST(testRetrieveInterfaceAdressFromAddress);
    CPPUNIT_TEST(testRetrieveInterfaceAdressFromAddressThrow);
    CPPUNIT_TEST(testGetIpV4InterfaceAddresses);
    CPPUNIT_TEST(testGetIpV4InterfaceAddressesLocal);
    CPPUNIT_TEST(testGetIpV4InterfaceAddressesPrivate);
    CPPUNIT_TEST(testGetAPrivateNetworkInterface);

    CPPUNIT_TEST_SUITE_END();

public:
    testCInterfaces();
    virtual ~testCInterfaces();
    void setUp();
    void tearDown();

private:
    void testRetrieveInterfaceAdressFromAddress();
    void testRetrieveInterfaceAdressFromAddressThrow();
    void testGetIpV4InterfaceAddresses();
    void testGetIpV4InterfaceAddressesLocal();
    void testGetIpV4InterfaceAddressesPrivate();
    void testGetAPrivateNetworkInterface();
};

#endif /* TESTCINTERFACES_H */

