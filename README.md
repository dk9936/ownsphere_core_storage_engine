# OwnSphere Storage Engine 🚀

A high-performance, distributed storage system inspired by NAS solutions like Synology.

## 🧱 Architecture Overview

### 🔧 Core Storage Engine (C++)

Handles:

* File chunking
* RAID logic (striping, mirroring - WIP)
* Encryption/Decryption (AES / ChaCha20 - planned)
* Disk I/O operations

### 🌐 Agent Layer (Go)

Handles:

* Communication with backend services
* File upload/download APIs
* Chunk management
* Health monitoring

---

## ✅ What We Have Achieved

* ✔️ Initial project setup (C++ + Go)
* ✔️ Basic file handling (read/write)
* ✔️ Agent service setup in Go
* ✔️ API structure defined
* ✔️ Local environment setup (Linux)
* ✔️ PostgreSQL removed & environment cleaned
* ✔️ Go installed using official binaries
* ✔️ Basic testing workflow established

---

## 📁 Project Structure

```
ownsphere/
│
├── core/                # C++ storage engine
│   ├── chunker/
│   ├── io/
│   └── encryption/
│
├── agent/               # Go service
│   ├── api/
│   ├── service/
│   └── utils/
│
├── scripts/             # Setup & test scripts
└── README.md
```

---

## 🔌 API Endpoints (Go Agent)

### 1. Upload File

```
POST /upload
```

**Description:**
Uploads a file → splits into chunks → stores via core engine.

**Request:**

* multipart/form-data
* file: binary

**Response:**

```json
{
  "status": "success",
  "file_id": "abc123"
}
```

---

### 2. Download File

```
GET /download/{file_id}
```

**Description:**
Reconstructs file from chunks and returns it.

---

### 3. Health Check

```
GET /health
```

**Response:**

```json
{
  "status": "ok"
}
```

---

### 4. Delete File (Planned)

```
DELETE /file/{file_id}
```

---

## 🧪 Testing Commands

### ▶️ Run Go Agent

```bash
cd agent
go run main.go
```

---

### 📤 Test Upload API

```bash
curl -X POST http://localhost:8080/upload \
  -F "file=@testfile.txt"
```

---

### 📥 Test Download API

```bash
curl -O http://localhost:8080/download/abc123
```

---

### ❤️ Health Check

```bash
curl http://localhost:8080/health
```

---

## ⚙️ Build Instructions

### Build C++ Core

```bash
cd core
mkdir build && cd build
cmake ..
make
```

---

### Run Go Agent

```bash
cd agent
go mod tidy
go run main.go
```

---

## 🛠️ Tech Stack

* C++ → Core storage engine
* Go → Agent / API layer
* Linux → Development environment
* Docker → (Planned)

---

## 🚧 Upcoming Features

* RAID implementation (0,1)
* Encryption layer (AES-256)
* Distributed node sync
* Metadata database integration
* Web dashboard

---

## 🧠 Design Goals

* High performance (low latency I/O)
* Scalability (multi-node support)
* Fault tolerance
* Secure storage

---

## 🤝 Contribution

This is an experimental system under active development.

---

## 📌 Notes

* This is NOT production ready yet
* Focus is on learning + building core infra

---

## 🚀 Author

Dinesh Kumar
OwnSphere Project
