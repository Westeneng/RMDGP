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
 * File:   CTime.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 9, 2021, 8:27 AM
 */

#ifndef CTIME_H
#define CTIME_H

#include <time.h>
#include <ostream>


class CTime : public timespec {
public:
    CTime();
    CTime(time_t sec, long nsec=0);
    CTime(const CTime& orig);
    CTime& operator=(const CTime& other);
    CTime operator-(const CTime& other) const;
    CTime& operator-=(const CTime& other);
    CTime operator+(const CTime& other) const;
    CTime& operator+=(const CTime& other);
    CTime operator*(int count) const;
    bool operator==(const CTime& other) const;
    bool operator!=(const CTime& other) const;
    bool operator>(const CTime& other) const;
    bool operator<(const CTime& other) const;
    void enforceNsecRange();
    
    friend std::ostream& operator<<(std::ostream& os, const CTime &time);
    
    virtual ~CTime();
    
    static constexpr long nsecInSec = 1000000000;
    static constexpr long nsecInMillisec = 1000000;
    static constexpr long nsecInMicrosec = 1000;
private:

};

#endif /* CTIME_H */

