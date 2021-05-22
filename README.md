# Reliable Multicast DataGram Protocol (RMDGP)

> A protocol that transports datagrams in a reliable way to multiple destinations.
> RMDGP only supports the transfer of small datagrams. Another layer(s) have to be added to enable bigger messages and secure transfer.
>

> __!!! WORK IN PROGRESS. Doesn't function yet !!!__

## Environment/prerequisites/application
> Initially I have a private network with a maximum of thousand receivers in mind. For now I do not have the intention to let this work on the public internet or with a huge amount of receivers.

## What does reliable mean in this context?
> All datagrams will be conveyed to all receivers in the same order as send.  
> However datagrams can be send multiple times over the network, for the sake that a receiver missed a message, at the application level only one datagram will be received.

## How
- The slowest receiver determines the speed of data transfer
- By specifing a minimum transfer speed, it is ensured that a single receiver with problems can't block the transfer.
The corresponding receiver will be declared as disconnected and will not have any influence on the behaviour of the sender anymore.
- by UDP/IP multicast

