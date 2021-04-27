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
 * File:   testclassCTimer.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on Apr 9, 2021, 4:13:36 PM
 */

#ifndef TESTCLASSCTIMER_H
#define TESTCLASSCTIMER_H

#include <cppunit/extensions/HelperMacros.h>
class CTime;

class testCTime : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testCTime);

    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testSubtract);
    CPPUNIT_TEST(testAdd);
    CPPUNIT_TEST(testCopy);
    CPPUNIT_TEST(testEqual);
    CPPUNIT_TEST(testMultiply);

    CPPUNIT_TEST_SUITE_END();

public:
    testCTime();
    virtual ~testCTime();
    void setUp();
    void tearDown();

private:
    void testConstructor();
    void testSubtract();
    void tstSubtractDataDriven(const std::string testName,
                const CTime &testTime, const CTime &value, const CTime &expectedResult);
    void testAdd();
    void tstAddDataDriven(const std::string testName,
                const CTime &testTime, const CTime &value, const CTime &expectedResult);
    void testCopy();
    void testEqual();
    void tstEqualDataDriven(const std::string testName, 
                    const CTime &value1, const CTime &value2, const bool expectedResult);
    void testMultiply();
    void tstMultiplyDataDriven(const std::string testName,
                    const CTime &value, int times, const CTime &expectedResult);
};

#endif /* TESTCLASSCTIMER_H */

