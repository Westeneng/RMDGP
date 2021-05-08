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
 * File:   CFdWaiter.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 30, 2021, 5:46 PM
 */

#ifndef CFDWAITER_H
#define CFDWAITER_H

#include "CTime.h"
#include <set>
#include <memory>

class CFileDescriptor;
class CSocketProxy;

class CFdWaiter {
public:
    CFdWaiter();
    CFdWaiter(const CFdWaiter& orig) = delete;
    CFdWaiter& operator=(const CFdWaiter& other) = delete;
    virtual ~CFdWaiter();

    void addReadFileDescriptor(const CFileDescriptor *fileDesriptor);
    void delReadFileDescriptor(const CFileDescriptor *fileDesriptor);
    void addWriteFileDescriptor(const CFileDescriptor *fileDesriptor);
    void delWriteFileDescriptor(const CFileDescriptor *fileDesriptor);

    /// \brief does the appropriate call to pselect with the current added fileDescriptors
    /// \return the returned value of the select system call
    /// \warning you have to handle the OS errors when select returns -1
    int select(const CTime &timeout);

    /// \brief wait until moment or a file descriptor is ready
    /// \param moment is a particular time of the monotonic clock (CLOCK_MONOTONIC)
    /// \return true if moment is passed. false if a file descriptor is ready for reading or
    ///         writing
    /// \throws std::runtime_error when OS reports an error. This can only happen when it is not
    ///         possible to allocate memory or when waitUntil has a bug.
    bool waitUntil(const CTime &moment);

    size_t getNumberReadFileDescriptors() const { return readFileDescriptors.size(); }
    size_t getNumberWriteFileDescriptors() const { return writeFileDescriptors.size(); }

    /// \brief sets the internal socket proxy object. For testing purpose only!
    /// \warning  DON'T USE THIS
    void setSocketProxy(std::shared_ptr<CSocketProxy> sockProxy);

private:
    fd_set readFdSet;
    fd_set writeFdSet;
    std::set<const CFileDescriptor*> readFileDescriptors;
    std::set<const CFileDescriptor*> writeFileDescriptors;

    std::shared_ptr<CSocketProxy> proxy;
};

#endif /* CFDWAITER_H */
