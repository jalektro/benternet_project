# Benternet Services using ZMQ in Qt Creator (C++)

## 1. Task Management System

### Overview

The Task Management System allows users to create, update, and delete tasks over Benternet. Each task includes a title, description, priority, and deadline. Clients can subscribe to receive updates when a task is modified.

### Architecture

- **Publisher**: Sends updates when tasks are added, modified, or deleted.
    
- **Subscriber**: Listens for task updates and synchronizes the task list.
    
- **Request-Reply**: Used for direct task queries (e.g., retrieving all tasks).
    

### Features

- Add new tasks with priority and deadline.
    
- Update existing tasks.
    
- Delete tasks.
    
- Retrieve all tasks.
    
- Notify clients of task changes.
    

### Implementation

- Use **ZMQ PUB-SUB** for task updates.
    
- Use **ZMQ REQ-REP** for fetching and modifying tasks.
    
- Store tasks in a **SQLite database** or an in-memory data structure.
    

## 2. Remote File Transfer Service

### Overview

This service allows clients to upload and download files over Benternet. It splits files into chunks for efficient transfer.

### Architecture

- **Client**: Requests to upload or download a file.
    
- **Server**: Manages file storage and handles requests.
    

### Features

- Upload files in chunks.
    
- Download files in chunks.
    
- List available files.
    
- Resume interrupted transfers.
    

### Implementation

- Use **ZMQ PUSH-PULL** for file transfer.
    
- Use **ZMQ REQ-REP** for file metadata requests (e.g., list files, check file size).
    
- Store files on a **local filesystem** or a **networked storage system**.
    

## 3. Beer Service (Existing)

### Overview

The Beer Service allows users to request and recommend beers. Users can search for beers, get random recommendations, and log their favorite beers.

### Architecture

- **Publisher**: Sends beer recommendations.
    
- **Subscriber**: Listens for new beer suggestions.
    
- **Request-Reply**: Queries for beer details.
    

### Features

- Get a random beer recommendation.
    
- Search for a specific beer.
    
- Rate and review beers.
    
- Retrieve user beer preferences.
    

### Implementation

- Use **ZMQ PUB-SUB** for beer recommendations.
    
- Use **ZMQ REQ-REP** for searching and rating beers.
    
- Store beer data in a **database** or a **local JSON file**.
    

---