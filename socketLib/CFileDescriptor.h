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
 * File:   CFileDescriptor.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 29, 2021, 7:42 PM
 */

#ifndef CFILEDESCRIPTOR_H
#define CFILEDESCRIPTOR_H

#include <memory>

class CSocketProxy;


class CFileDescriptor {
public:
    CFileDescriptor();
    CFileDescriptor(std::shared_ptr<CSocketProxy> sockProxy);
    CFileDescriptor(const CFileDescriptor& orig) = delete;
    virtual ~CFileDescriptor();

    /// \brief returns true if the object is successfully opened
    bool isOpen() const { return sfd >= 0; }

    /// \brief sets the internal socket proxy object. For testing purpose only!
    /// \warning  DON'T USE THIS
    void setSocketProxy(std::shared_ptr<CSocketProxy> sockProxy);

protected:
    int sfd;     // socket file descriptor
    std::shared_ptr<CSocketProxy> proxy;
};

#endif /* CFILEDESCRIPTOR_H */

