# About
Distributed Data Processing application (Windows only) that utilizes multiple computers to execute an algorithm on a set of data. The clients will then send the processsed data back to the server so it can be reordered and combined. This speeds up the processing and analysis of data drastically over using a single computer.  

Custom algorithms and custom data can be created/generated for the clients to process/anyalze.

### Setup
* Make sure TCPCS.dll, TCPCS.lib, zlib1.dll are all in the executable directory before running.
* Run DataGen to generate data for the clients to process
* Start the server
* Allow connections on the server
* Start the client(s)
* Connect the clients and recvdata

### Dependencies
- WNLIB - https://github.com/CameronClay/WNLIB-Chat

### Unit Testing
- Unit testing done with Google Test.

### Tooling
- C++
- Boost.LockFree
- Google Test
- Win32 API
- Visual Studio

#### Server usage
```
Commands: help, exit, allowcon, shutdown  
Server> allowcon -h  
Usage: -p=Port [-ipv=ipv4/ipv6/ipboth[ipv4] -nt=nThreads[8] -nct=nConcurrentThreads[4] -nca=nConcurrentAccepts[4]]
```

#### Client usage
```
Commands: help, exit, connect, disconnect, recvdata  
Client> connect -h  
Usage: -ip=IP address -p=port -ipv6=true/false[false] [-t=timesec[5.0]]  
Client> recvdata -h  
Usage: [-nt=nThreads[2] -nct=nConcThreads[1]]  
```

##### Disclamer
Use at own risk. NO warranty is provided whatsoever.
