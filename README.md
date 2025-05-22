# Benternet ZMQ Client & Beer Service

A Qt‑based C++ client for the Benternet message network over ZeroMQ, paired with the **Beer Service** that recommends Belgian beers. This unified README covers both the general client functionality and the beer‑specific protocol.

---

## 🍻 Overview

* **Client**: Push/pull messaging to any Benternet service using `PUSH` and `SUB` sockets.
* **Beer Service**: A sample Benternet service that replies with random Belgian beers or stats.

Features:

1. **Push Requests**: `service?>payload`, delivered via ZMQ\_PUSH.
2. **Explicit Subscribe/Unsubscribe**: Listen to topic prefixes (`SUBSCRIBE`/`UNSUBSCRIBE`).
3. **One-shot Requests**: Auto‑subscribe, send once, auto‑unsubscribe after first reply.
4. **Subscribe All Toggle**: Listen to *all* messages (empty prefix).
5. **Beer Protocol**: Specialized formats and replies for the beer example.

---

## 🧰 Prerequisites

* C++17 compiler (e.g. g++, MSVC)
* Qt 5 or 6 (Core, GUI, Widgets)
* ZeroMQ (libzmq) and cppzmq headers
* Connectivity to `benternet.pxl-ea-ict.be` (24041 PUSH, 24042 SUB)

---

## 🔨 Building the Client

```bash
mkdir build && cd build
cmake ..
make
```

## ▶️ Running

```bash
./benternet-client
```

The UI provides:

* **Input field** for commands or topics.
* **Send** button to push requests.
* **Subscribe** / **Unsubscribe** buttons for topic prefixes.
* **Subscribe All** toggle to hear every message.
* **Log** pane for sent, received, and status messages.

---

## 📜 Commands & Usage

### Generic Push Request

```
<service?>payload
```

* E.g., `weather?>Brussels`.
* Auto‑subscribe to `<service>` if one-shot mode is enabled.

### Explicit Subscribe / Unsubscribe

```
subscribe?>topicName
unsubscribe?>topicName
```

* Starts or stops listening on `topicName` prefix.

### One-shot Mode (inside Send)

On sending a generic request, the client auto‑subscribes to `<service>` prefix, sends the message, receives the first matching reply, then auto‑unsubscribes.

### Subscribe All Toggle

* Check to subscribe to *every* message (empty prefix).
* Uncheck to unsubscribe from everything.

---

## 🍺 Beer Service Protocol

A concrete example service running on Benternet that recommends beers.

### Connection Setup

```cpp
zmq::context_t context(1);
zmq::socket_t pusher(context, ZMQ_PUSH);
zmq::socket_t subscriber(context, ZMQ_SUB);

subscriber.connect("tcp://benternet.pxl-ea-ict.be:24042");
subscriber.setsockopt(ZMQ_SUBSCRIBE, "beer", 4);

pusher.connect("tcp://benternet.pxl-ea-ict.be:24041");
```

### Request Formats

| Command        | Description                     |
| -------------- | ------------------------------- |
| `beer?>[name]` | Get a personalized beer         |
| `beer?>stats`  | Get per-user request statistics |
| `beer?>help`   | Show available commands         |

### Response Formats

| Reply                                        | Meaning                          |
| -------------------------------------------- | -------------------------------- |
| `beer!>[name], your beer is: [beername]`     | Beer recommendation for `[name]` |
| `beer!>Stats: name1:count1, name2:count2...` | Lifetime request counts          |
| `beer!>Commands: help, [name], stats`        | Supported commands               |

---

## 🧾 Beer Example Walkthrough

1. **One-shot request**:

   ```text
   beer?>tom
   ```

2. **Log output**:

   ```text
   → One-shot subscribe to: beer
   Sending: beer?>tom
   Received: beer!>tom, your beer is: Westmalle
   ← One-shot unsubscribe from: beer
   ```

3. **Stats request**:

   ```text
   beer?>stats
   ```

4. **Log output**:

   ```text
   Sending: beer?>stats
   Received: beer!>Stats: tom:3, jan:5, lisa:1
   ```

---

## 📦 Code Structure

* **`main.cpp`**: Qt application bootstrap.
* **`mainwindow.h/cpp`**: UI, slot wiring, subscribe/unsubscribe logic.
* **`clientworker.h/cpp`**: ZMQ sockets (SUB + PUSH), subscription helpers.

---

## 🔮 Future Improvements

* Persistent stats storage (e.g., SQLite)
* User profiles and preferences
* Enhanced GUI with filters and charts

---

## 📄 License

MIT License © 2025
