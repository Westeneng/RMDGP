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
 * File:   CUdpSocket.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 23, 2021, 2:00 PM
 */

#ifndef CUDPSOCKET_H
#define CUDPSOCKET_H

#include <memory>

class CSocketProxy;

class CUdpSocket {
public:
    CUdpSocket();
    CUdpSocket(std::shared_ptr<CSocketProxy> sockProxy);
    CUdpSocket(const CUdpSocket& orig) = delete;
    virtual ~CUdpSocket();

    /// \brief Sets the object (and internal file descriptor) in non blocking mode
    /// \throws std::runtime_error when non blocking mode is not available.
    void setNonBlocking() const;

    /// \brief opens a UPD datagram socket and stores the file descriptor in sfd.
    ///        If there is a valid file descriptor this will be closed first to prevent use from
    ///        leaking resources.
    /// \throws std::runtime_error when OS reports an error.
    void openUdpSocket();

    /// \brief closes the socket file descriptor
    /// \throws std::runtime_error when OS reports an error.
    void closeUdpSocket();

    /// \brief returns true if the object is successfully opened
    bool isOpen() const;

    /// \brief sets the internal socket proxy object. For testing purpose only!
    /// \warning  DON'T USE THIS
    void setSocketProxy(std::shared_ptr<CSocketProxy> sockProxy);

protected:
    int sfd;     // socket file descriptor
    std::shared_ptr<CSocketProxy> proxy;
};

#endif /* CUDPSOCKET_H */

