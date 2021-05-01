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
 * File:   CClock.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 30, 2021, 8:03 PM
 */

#ifndef CCLOCK_H
#define CCLOCK_H

class CTime;

/// \brief an interface that retrieves the clock times from the OS
class CClock {
public:
    CClock();
    CClock(const CClock& orig) = delete;
    virtual ~CClock();

    /// \brief retrieve the monotonicTime from OS.
    /// \param monotonicTime object that receives the time
    /// \returns a reference to monotonicTime
    /// \throws std::runtime_error when clock points outside the accessible address space or when
    ///         The OS doesn't support CLOCK_MONOTONIC
    static CTime& getMonotonicTime(CTime &monotonicTime);
private:

};

#endif /* CCLOCK_H */

