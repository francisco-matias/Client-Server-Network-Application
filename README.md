# Distributed Tree-Based Content Routing Network

A distributed content discovery system implemented in C, featuring dynamic tree topology management, custom routing logic, and concurrent TCP/UDP communication using low-level POSIX sockets.

---

## Overview

This project implements a distributed overlay network where nodes are organized in a dynamic tree topology and collaboratively perform content discovery and routing.

Each node maintains forwarding information learned automatically through message propagation, enabling efficient query-based content retrieval across the network.

The system supports node join and leave operations, topology recovery, and dynamic routing updates without centralized coordination.

---

## Core Concepts Demonstrated

- Distributed systems design  
- Overlay network topology management  
- Dynamic routing-table construction  
- Peer-to-peer communication over TCP  
- Directory services over UDP  
- I/O multiplexing with `select()`  
- Fault-tolerant network recovery  
- Low-level network programming in C  

---

## Technical Skills Applied

- POSIX socket programming  
- Concurrent TCP and UDP handling  
- Custom application-layer protocol design  
- Manual message parsing and formatting  
- Robust error handling and connection management  
- Modular systems architecture  

---

## Technologies

- C  
- TCP / UDP  
- POSIX sockets  
- `select()` multiplexing  

