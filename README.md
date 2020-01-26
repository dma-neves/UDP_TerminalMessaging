# UDP_TerminalMessaging

**Description:**
  - A simple terminal messaging platform
  - The platform contains 2 programs, a server and a client. Once the server is running on a certain machine, 
  any other machine, on the same network given the hostname (or ip address) and port of the machine running 
  the server, is able to run the client program. From the client program it is possible to register a user/account
  and send and receive messages from other users registered on the platform

**Requirements:**
  1. Unix based operating system
  2. GCC compiler
  
**Building and running**
  1. *Server*
      - To build the server program use the given makefile
      - To run the server use `./server.out serverPort`
     
  2. *Client*
      - To build the client program use the given makefile
      - To run the client use `./client.out serverHostName serverPort clientPort`
      
**Notes**
  - If your running both programs on the same machine use localhost as the hostName
  - All the clients (and server) running on the same machine must be initialized with different ports 
