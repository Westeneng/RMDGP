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
 * File:   testclassCTimer.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on Apr 9, 2021, 4:13:36 PM
 */

#include "testCTime.h"
#include "../CTime.h"
#include <sstream>


CPPUNIT_TEST_SUITE_REGISTRATION(testCTime);

testCTime::testCTime()
{
}

testCTime::~testCTime()
{
}

void testCTime::setUp()
{
}

void testCTime::tearDown()
{
}

void testCTime::testConstructor()
{
   CTime time;

   CPPUNIT_ASSERT(time.tv_sec == 0);
   CPPUNIT_ASSERT(time.tv_nsec == 0);
   
   CTime time2(3);
   CPPUNIT_ASSERT(time2.tv_sec == 3);
   CPPUNIT_ASSERT(time2.tv_nsec == 0);
   
   CTime time3(103050709, 987123456);
   CPPUNIT_ASSERT(time3.tv_sec == 103050709);
   CPPUNIT_ASSERT(time3.tv_nsec == 987123456);

   // test that constructor corrects out of range ns value
   CTime time4(103050709, 1987123456);
   CPPUNIT_ASSERT(time4.tv_sec == 103050710);
   CPPUNIT_ASSERT(time4.tv_nsec == 987123456);

   // test that constructor corrects not allowed negative value
   CTime time5(103050709, -2000123456);
   CPPUNIT_ASSERT_EQUAL(int(103050706), int(time5.tv_sec));
   CPPUNIT_ASSERT_EQUAL(long(999876544), time5.tv_nsec);
}

void testCTime::testCopy()
{
   const CTime orig(123,987);
   CTime copy(orig);
   CPPUNIT_ASSERT_EQUAL(time_t(123), copy.tv_sec);
   CPPUNIT_ASSERT_EQUAL(long(987), copy.tv_nsec);
   
   CTime assigned; // don't assign direct, because then the copy constructor will be used.
   assigned = orig;
   CPPUNIT_ASSERT_EQUAL(time_t(123), assigned.tv_sec);
   CPPUNIT_ASSERT_EQUAL(long(987), assigned.tv_nsec);
}

void testCTime::testSubtract()
{
   CPPUNIT_ASSERT(true);
   
   // be aware that the nano second part is always positive! so CTime(-3, 999999999)
   //  means -2.000000001 seconds
   CPPUNIT_ASSERT_NO_THROW(
      tstSubtractDataDriven("Subtract seconds", CTime(185,1000), CTime(5), CTime(180,1000)) );
   CPPUNIT_ASSERT_NO_THROW(
      tstSubtractDataDriven("Subtract seconds negative", CTime(185,10), CTime(200), CTime(-15,10)) );
   CPPUNIT_ASSERT_NO_THROW(
      tstSubtractDataDriven("Subtract nanoseconds at boundary", CTime(185,1000), CTime(1,1000), CTime(184,0)) );
   CPPUNIT_ASSERT_NO_THROW(
      tstSubtractDataDriven("Subtract nanoseconds over boundary", CTime(185,1000), CTime(1,1001), CTime(183,999999999)) );
   CPPUNIT_ASSERT_NO_THROW(
      tstSubtractDataDriven("Subtract nanoseconds over boundary negative", CTime(18,1000), CTime(20,1001), CTime(-3,999999999)) );
   CPPUNIT_ASSERT_NO_THROW(
      tstSubtractDataDriven("Subtract nanoseconds over boundary negative2", CTime(-1,1000), CTime(1,1001), CTime(-3,999999999)) );
   CPPUNIT_ASSERT_NO_THROW(
      tstSubtractDataDriven("Subtract negative from negative", CTime(-1,1000), CTime(-1,1001), CTime(-1,999999999)) );
   CPPUNIT_ASSERT_NO_THROW(
      tstSubtractDataDriven("Subtract negative", CTime(10,1000), CTime(-1,1001), CTime(10,999999999)) );
   CPPUNIT_ASSERT_NO_THROW(
      tstSubtractDataDriven("Subtract negative2", CTime(999,100), CTime(-1,100), CTime(1000,0)) );
}

void testCTime::tstSubtractDataDriven(const std::string testName,
        const CTime &testTime, const CTime &value, const CTime &expectedResult)
{
   CTime result = testTime - value;
   
   if((result.tv_sec != expectedResult.tv_sec) || (result.tv_nsec != expectedResult.tv_nsec))
   {
      std::ostringstream message;
      message << testName << ", Result:" << result << " expected:" << expectedResult;
      throw(std::runtime_error(message.str()));
   }
   
   result = testTime;
   result -= value;

   if((result.tv_sec != expectedResult.tv_sec) || (result.tv_nsec != expectedResult.tv_nsec))
   {
      std::ostringstream message;
      message << testName << ", Result2:" << result << " expected:" << expectedResult;
      throw(std::runtime_error(message.str()));
   }
}

void testCTime::testAdd()
{
   tstAddDataDriven("Add Seconds", CTime(5), CTime(1999), CTime(2004,0));
   tstAddDataDriven("Add nano Seconds", CTime(5, 12345678), CTime(1999, 87654321), CTime(2004,99999999));
   tstAddDataDriven("Add nano Seconds overflow", CTime(5, 123456789), CTime(1999, 987654321), CTime(2005,111111110));
   tstAddDataDriven("Add nano Seconds negative", CTime(5, 12345678), CTime(-1999, 87654321), CTime(-1994,99999999));
   tstAddDataDriven("Add nano Seconds negative2", CTime(115, 123456789), CTime(-9, 876543210), CTime(106,999999999));
   tstAddDataDriven("Add nano Seconds negative overflow", CTime(5, 123456789), CTime(-1, 987654321), CTime(5,111111110));
}

void testCTime::tstAddDataDriven(const std::string testName,
                const CTime &testTime, const CTime &value, const CTime &expectedResult)
{
   CTime result = testTime + value;
   
   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName, expectedResult, result);
   
   result = testTime;
   result += value;

   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName+" +=", expectedResult, result);
}

void testCTime::testEqual()
{
   tstEqualDataDriven("Equal zero", CTime(), CTime(), true);
   tstEqualDataDriven("Equal only ns", CTime(0, 12345), CTime(0, 12345), true);
   tstEqualDataDriven("Equal only sec", CTime(444), CTime(444), true);
   tstEqualDataDriven("Equal", CTime(-9, 999), CTime(-9, 999), true);

   tstEqualDataDriven("Only nsec Not Equal", CTime(1, 777), CTime(1, 776), false);
   tstEqualDataDriven("Only sec Not Equal", CTime(-1, 777), CTime(1, 777), false);
   tstEqualDataDriven("Both Not Equal", CTime(1, 777), CTime(2, 776), false);
}

void testCTime::tstEqualDataDriven(const std::string testName, 
                    const CTime &value1, const CTime &value2, const bool expectedResult)
{
   const bool resultEqual = value1 == value2;
   const bool resultNotEqual = value1 != value2;
   std::ostringstream erorMessage;
   
   if(resultEqual != expectedResult)
   {
      erorMessage << testName << "(==) expected:" << expectedResult << " result:" << resultEqual
              << " value1=" << value1 << " value2=" << value2;
      
      CPPUNIT_ASSERT_MESSAGE(erorMessage.str(), false);
   }
   if(resultNotEqual == expectedResult)
   {
      erorMessage << testName << "(!=) expected:" << !expectedResult << " result:" << resultNotEqual
              << " value1=" << value1 << " value2=" << value2;
      
      CPPUNIT_ASSERT_MESSAGE(erorMessage.str(), false);
   }
}

void testCTime::testLessThan()
{
   tstLessThanDataDriven("equal value", CTime(0,1), CTime(0,1), false);
   tstLessThanDataDriven("equal value2", CTime(123456789), CTime(123456789), false);
   tstLessThanDataDriven("equal value3", CTime(-5,1), CTime(-5,1), false);
   tstLessThanDataDriven("only sec greater", CTime(1,1), CTime(0,14), false);
   tstLessThanDataDriven("only nsec greater", CTime(-1000,15), CTime(-1000,14), false);
   tstLessThanDataDriven("greater", CTime(1,150000), CTime(0,15), false);
   tstLessThanDataDriven("only sec smaller", CTime(11,999999999), CTime(12), true);
   tstLessThanDataDriven("only nsec smaller", CTime(11,0), CTime(11,1), true);
   tstLessThanDataDriven("smaller", CTime(-5544332211,0), CTime(11,123456789), true);
}

void testCTime::tstLessThanDataDriven(const std::string testName,
                    const CTime &value1, const CTime &value2, const bool expectedResult)
{
   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName, expectedResult, value1<value2);
}

void testCTime::testGreaterThan()
{
   tstGreaterThanDataDriven("equal value", CTime(0,1), CTime(0,1), false);
   tstGreaterThanDataDriven("equal value2", CTime(123456789), CTime(123456789), false);
   tstGreaterThanDataDriven("equal value3", CTime(-5,1), CTime(-5,1), false);
   tstGreaterThanDataDriven("only sec greater", CTime(1,0), CTime(0,999999999), true);
   tstGreaterThanDataDriven("only nsec greater", CTime(-1000,17), CTime(-1000,16), true);
   tstGreaterThanDataDriven("greater", CTime(1,150000), CTime(0,15), true);
   tstGreaterThanDataDriven("only sec smaller", CTime(11,999999999), CTime(12), false);
   tstGreaterThanDataDriven("only nsec smaller", CTime(11,0), CTime(11,1), false);
   tstGreaterThanDataDriven("smaller", CTime(-5544332211,0), CTime(11,123456789), false);
}

void testCTime::tstGreaterThanDataDriven(const std::string testName,
                    const CTime &value1, const CTime &value2, const bool expectedResult)
{
   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName, expectedResult, value1>value2);
}

void testCTime::testMultiply()
{
   tstMultiplyDataDriven("ns * 1000", 
               CTime(0,1), 1000, CTime(0, CTime::nsecInMicrosec));
   tstMultiplyDataDriven("us * 1000", 
               CTime(0,CTime::nsecInMicrosec), 1000, CTime(0, CTime::nsecInMillisec));
   tstMultiplyDataDriven("ms * 1000", 
               CTime(0,CTime::nsecInMillisec), 1000, CTime(1));
   tstMultiplyDataDriven("ms * 20000001", 
               CTime(0,CTime::nsecInMillisec), 20000001, CTime(20000,CTime::nsecInMillisec));
   tstMultiplyDataDriven("ms * -999", 
               CTime(0,CTime::nsecInMillisec), -999, CTime(-1,CTime::nsecInMillisec));
   tstMultiplyDataDriven("ms * -1000", 
               CTime(0,CTime::nsecInMillisec), -1000, CTime(-1));
   tstMultiplyDataDriven("ms * -1001", 
               CTime(0,CTime::nsecInMillisec), -1001, CTime(-2, 999000000));
   tstMultiplyDataDriven("100ms * -100000",
               CTime(0,100*CTime::nsecInMillisec), -100000, CTime(-10000));
}

void testCTime::tstMultiplyDataDriven(const std::string testName,
                    const CTime &value, int times, const CTime &expectedResult)
{
   CTime result = value * times;
   
   if(result != expectedResult)
   {
      std::ostringstream erorMessage;
      erorMessage << testName << " expected:" << expectedResult << " result:" << result << std::endl
              << "  value=" << value << "  times=" << times;
      
      CPPUNIT_ASSERT_MESSAGE(erorMessage.str(), false);      
   }
}


