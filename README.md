# 🍺 Beer Service - User Manual

  

## Overview

  

The **Beer Service** is a lightweight, networked service that recommends random Belgian beers over the Benternet protocol. It listens for beer requests and replies with a randomly chosen beer from a predefined list.

  

This guide explains how to use the service as a client.

  

---

  

## 🧰 Getting Started

  

### Prerequisites

  

- C++ compiler (e.g. g++, MSVC)

- [ZeroMQ (ZMQ)](https://zeromq.org/) library installed

- A working Benternet server connection (host: `benternet.pxl-ea-ict.be`)

- Basic knowledge of C++ and networking

  

---

  

## 🍻 Using the Beer Service

  

### Step 1: Set Up the Connection

  

Initialize your ZMQ context and sockets:

  

```cpp

zmq::context_t context(1);

zmq::socket_t pusher(context, ZMQ_PUSH);

zmq::socket_t subscriber(context, ZMQ_SUB);

  

subscriber.connect("tcp://benternet.pxl-ea-ict.be:24042");

subscriber.setsockopt(ZMQ_SUBSCRIBE, "beer?>", 6);

  

pusher.connect("tcp://benternet.pxl-ea-ict.be:24041");

```

  

---

  

### Step 2: Send a Beer Request

  

Clients can send a beer request using the following format:

  

```cpp

// Format: beer?>[username or command]

  

// Example:

std::string request = "beer?>jan";

pusher.send(zmq::message_t(request.data(), request.size()));

```

  

---

  

### Step 3: Receive the Response

  

Listen for responses from the service:

  

```cpp

zmq::message_t msg;

subscriber.recv(&msg);

std::string reply(static_cast<char*>(msg.data()), msg.size());

std::cout << "Beer Service Reply: " << reply << std::endl;

```

  

---

  

## 📢 Request Format

  

The Beer Service recognizes the following input formats:

  

- `beer?>get>[name]` — Get a personalized beer recommendation  

- `beer?>stats>` — Get current request statistics  

- `beer?>help>` — Show help and available commands

  

---

  

## 📦 Response Format

  

The service will respond with:

  

- `beer!>[name], your beer is: [beername]`  

- `beer!>Stats: name1:count1, name2:count2...`  

- `beer!>Commands: help, [your name], stats`  

  

---

  

## 🧾 Example

  

```text

// Request:

beer?>tom

  

// Response:

beer!>tom, your beer is: Westmalle

```

  

```text

// Request:

beer?>stats

  

// Response:

beer!>Stats: tom:3, jan:5, lisa:1

```

  

---

  

## ❌ Error Handling

  

Currently, this service does not send error messages. Malformed topics are simply ignored.

  

---

  

## 🔧 Implementation Notes

  

- Uses `ZMQ_PUSH` and `ZMQ_SUB` sockets for communication

- Beer list is randomly chosen from a hardcoded vector

- Request counts are tracked in memory using a `std::map`

  

---

  

## 🧪 Future Improvements (Optional)

  

- Persistent tracking with SQLite  

- User-submitted ratings and preferences  

- GUI front-end (Qt-based)

  

---
