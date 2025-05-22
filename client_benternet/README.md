# First Level Heading

Paragraph.

## Second Level Heading

Paragraph.

- bullet
+ other bullet
* another bullet
    * child bullet

1. ordered
2. next ordered

### Third Level Heading

Some *italic* and **bold** text and `inline code`.

An empty line starts a new paragraph.

Use two spaces at the end  
to force a line break.

A horizontal ruler follows:

---

Add links inline like [this link to the Qt homepage](https://www.qt.io),
or with a reference like [this other link to the Qt homepage][1].

    Add code blocks with
    four spaces at the front.

> A blockquote
> starts with >
>
> and has the same paragraph rules as normal text.

First Level Heading in Alternate Style
======================================

Paragraph.

Second Level Heading in Alternate Style
---------------------------------------

Paragraph.

[1]: https://www.qt.io
# Benternet ZMQ Client

This is a C++ Qt-based client for interacting with the Benternet message network over ZeroMQ. It supports:

* **Push requests** to services (`service?>payload`)
* **Explicit subscribe/unsubscribe** to topics
* **One-shot** requests (auto-subscribe, push, then auto-unsubscribe)
* **Subscribe All** toggle (subscribe to *all* messages)

The **beer** example is included: a simple `beer` service that replies with your assigned beer.

---

## Features

1. **Send Request**: Push a `service?>payload` request to the network.
2. **Subscribe**: Listen to a specific topic prefix (e.g. `beer`).
3. **Unsubscribe**: Stop listening to a specific topic.
4. **One-shot**: The client will auto-subscribe to the service prefix, send the request, receive the first reply, then auto-unsubscribe.
5. **Subscribe All**: Toggle listening on **all** topics (empty prefix).

## Requirements

* Qt 5 or 6 (Core, GUI, Widgets)
* ZeroMQ (libzmq) and cppzmq headers
* C++17 compiler

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running

```bash
./benternet-client
```

## UI Layout

* **Input field**: Enter commands or topics.
* **Send**: Pushes the text as-is (`service?>payload` or `topic`).
* **Subscribe**: Subscribes to the topic in the input.
* **Unsubscribe**: Unsubscribes from the topic.
* **Subscribe All**: Checkable button to subscribe/unsubscribe to all messages.
* **Log**: Displays sent requests, received replies, and status.

## Commands

### Push Request

```text
beer?>robert
```

* Auto-subscribes to `beer` prefix (one-shot)
* Sends `beer?>robert`
* Receives: `beer!>robert, your beer is: Orval, Kasteelbier`

### Subscribe / Unsubscribe

```text
subscribe?>weather
unsubscribe?>weather
```

* Subscribes/unsubscribes to `weather` prefix.

### One-shot Example

1. In input, type:

   ```text
   beer?>robert
   ```
2. Log shows:

   ```text
   → One-shot subscribe to: beer
   Sending: beer?>robert
   Received: beer!>robert, your beer is: Orval, Kasteelbier
   ← One-shot unsubscribe from: beer
   ```

### Subscribe All Toggle

* Click **Subscribe All** to start receiving *all* messages.
* Click again (now **Unsubscribe All**) to stop.

## Code Structure

* **`main.cpp`**: Application entry point.
* **`mainwindow.h/cpp`**: UI, input handling, ZMQ subscribe logic.
* **`clientworker.h/cpp`**: ZeroMQ sockets (SUB + PUSH), subscribe/unsubscribe helpers.

## License

MIT License. Feel free to copy, modify, and distribute.
