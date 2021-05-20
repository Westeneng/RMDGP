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

#include "CFileDescriptor.h"

struct in_addr;
struct sockaddr_in;


class CUdpSocket : public CFileDescriptor {
public:
    CUdpSocket();
    CUdpSocket(std::shared_ptr<CSocketProxy> sockProxy);
    CUdpSocket(const CUdpSocket& orig) = delete;
    virtual ~CUdpSocket();

    /// \brief Sets the object (and internal file descriptor) in non blocking mode
    /// \throws std::runtime_error when non blocking mode is not available.
    void setNonBlocking() const;

    /// \brief bind to the interface and port. If the interface doesn't exist or the port is not
    ///        available the OS will report an error, and closeAndThrowRuntimeException will be
    ///        called.
    /// \param interfaceAddress
    ///        the interface address to bind with. or INADDR_ANY if you want to bind to all
    ///        interfaces
    /// \param port
    ///        the port to bind with. If set to 0 then OS will select an appropriate port, which
    ///        can be retrieved with getLocalPort()
    /// \throws std::runtime_error when OS reports an error.
    /// \warning when the bind fails, the socket will be closed.
    void bind(const in_addr interfaceAddress, int port);

    /// \brief returns the port at which the socket is bound
    /// \throws std::runtime_error when OS reports an error.
    int getLocalPort() const;

    /// \brief returns the ip address at which the socket is bound
    /// \throws std::runtime_error when OS reports an error.
    in_addr getLocalAddress() const;

    /// \brief returns the socket address at which the socket is bound
    /// \throws std::runtime_error when OS reports an error.
    /// \warning be aware that port number is in network byte order
    sockaddr_in getLocalSockAddress() const;

    /// \brief opens a UPD datagram socket and stores the file descriptor in sfd.
    ///        If there is a valid file descriptor this will be closed first to prevent use from
    ///        leaking resources.
    /// \throws std::runtime_error when OS reports an error.
    void openUdpSocket();

    /// \brief closes the socket file descriptor
    /// \throws std::runtime_error when OS reports an error.
    void closeUdpSocket();

    /// \brief sends a message to the destination
    /// \return the number of bytes send. If the socket is in non blocking mode and the socket would
    ///         block then 0 will be returned.
    /// \throws std::runtime_error when OS reports an error that cannot be handled.
    size_t sendTo(const void *buffer, size_t bufferSize, sockaddr_in *destination);

    /// \brief receives an udp message from the socket.
    /// \param buffer the buffer that receives the message
    /// \param bufferSize, the size of buffer
    /// \param source, buffer that will receive the source address of the message.
    /// \return the number of bytes received. Will be 0 when socket is in non blocking mode and
    ///         no messages are available.
    /// \throws std::runtime_error when OS reports an error.
    size_t receiveFrom(void *buffer, size_t bufferSize, sockaddr_in *source);

    /// \brief helper function that closes the socket and assembles and throws a std::runtime_error
    ///        exception. The what() message is composed from the given matter and the available
    ///        errno. This is meant for error handling when OS reports an error.
    /// \throws always std::runtime_error
    void closeAndThrowRuntimeException(const std::string matter);
};

#endif /* CUDPSOCKET_H */

