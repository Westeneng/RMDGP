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
 * File:   CUdpMulticastSender.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on March 18, 2021, 5:08 PM
 */

#ifndef CUDPMULTICASTSENDER_H
#define CUDPMULTICASTSENDER_H

#include "CUdpSocket.h"
#include <memory>
#include <string>
#include <stddef.h>

class CSocketProxy;
struct sockaddr_in;
struct in_addr;

/// \brief Provides the ability to send messages to a TCP/UDP multi cast group
class CUdpMulticastSender : public CUdpSocket {
public:
    CUdpMulticastSender();
    CUdpMulticastSender(const CUdpMulticastSender& orig) = delete;
    virtual ~CUdpMulticastSender();

    /// \brief Open for sending. The internal socket will be created and initialized.
    /// \param multicastAddress
    ///        The multi cast address to send to.
    /// \param multicastPort
    ///        The multi cast port to send to.
    /// \param interfaceAddress
    ///        address of the interface to bound to.
    /// \param localPort
    ///        port to bound to. If set to 0 the OS will choose an appropriate port.
    /// \throws std::runtime_error when not able to open and initialize the socket.
    ///         If no exception is thrown then the socket is successfully opened.
    void open(const std::string multicastAddress, int multicastPort,
                const std::string interfaceAddress, int localPort=0);
    void open(const in_addr& multicastAddress, int multicastPort,
                const in_addr& interfaceAddress, int localPort=0);

    /// \brief Send a message to the multi cast address.
    /// \return The number of bytes send. 
    ///         In non blocking mode 0 will be returned when the socket would block.
    /// \throws std::runtime_error when OS reports an error.
    size_t send(const void *buffer, size_t bufferSize);

    /// \brief returns the port at which the sender socket is bound
    /// \throws std::runtime_error when OS reports an error.
    int getSenderPort() const { return CUdpSocket::getLocalPort(); }
    /// \brief returns the multicast ip address
    in_addr getMulticastIpAddress() const;
    /// \brief returns the multicast port
    int getMulticastPort() const;
    
private:
    std::unique_ptr<sockaddr_in> multicastDestination;
};

#endif /* CUDPMULTICASTSENDER_H */

