# Distributed Tree-Based Content Routing Network

This project implements a TCP-based client–server system in C using POSIX sockets.

The application establishes a network connection between a client and a server, enabling structured message exchange and server-side request processing.

## Architecture

The system follows a classic client-server model:

- **Server**
  - Creates a TCP socket
  - Binds to a port and listens for incoming connections
  - Accepts client connections
  - Processes client requests
  - Sends structured responses back to the client

- **Client**
  - Establishes a TCP connection to the server
  - Sends requests following the defined protocol
  - Receives and processes server responses

# Features

- TCP communication using POSIX sockets
- Connection establishment and teardown
- Structured message exchange protocol
- Robust error handling
- Clear separation between client and server logic
- Low-level systems programming in C
