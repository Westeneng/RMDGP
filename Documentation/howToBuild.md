# How to build RMDGP

> This guide expects you have a linux installation to build 
> Reliable Multicast DataGram Protocol (RMDGP).

## Required packages
>
> To build RMDGP you need the following packages:
> - gcc
> - g++
> - gmake
> - cmake
>
> To build the unit tests you need:
> - libcppunit-dev (Ubuntu 20.10, CppUnit unit testing framework)
>
> To see the .md files in documentation in a nice format you can install grip
>
> depending on your distro use yum or apt to install them.

## Steps

1. Open a terminal
1. Create a direcotory where you want to build
1. cd to the dir
1. run cmake with the path to the root dir of your RMDGP git repo
> *example:*  
> cmake /home/username/code/RMDGP/
>
> *or in case you want a debug build:*  
> cmake -DCMAKE_BUILD_TYPE=Debug /home/username/code/RMDGP/
5. build the sources with gmake
> gmake -j10

