# Vfs-lite

### **Component Overview**
### 1. Virtual File System (VFS)

**Role**:
- The abstraction layer between user-facing file operations and the underlying filesystems.

**Responsibilities**:
- Register/unregister filesystems
- Mount/unmount file systems
- Route file operations like open, read, write, ls to the appropriate driver
- Provide a unified API for accessing files

### 2. Filesystem Drivers

- These implement the actual logic for handling file formats and metadata on disk.

#### In-Memory Filesystem (e.g., RamFS or TempFS-like)**
**Role**: A simple filesystem that exists only in RAM.

**Responsibilities**:
- File creation/deletion
- Storing file contents in memory (likely hash maps or flat structs)
- No persistence — for fast access and testability

#### Disk-Based Filesystem (e.g., ext2 Read-Only)
**Role**: A parser and accessor for a real filesystem format stored in a file or disk image.

**Responsibilities**:
- Parse superblock, inode tables, directory entries
- Handle block addressing
- Support basic operations: ls, cat, stat (read-only)

###  **Unit Testing**
| Component    | What to test                      | How                       |
| ------------ | --------------------------------- | ------------------------- |
| VFS          | Mounting logic, routing dispatch  | Stub/mock filesystem APIs |
| In-Memory FS | File creation, lookup, read/write | Allocate dummy files      |
| ext2 Driver  | Superblock parsing, inode lookup  | Use static image files    |




```sql
              +--------------------------+
              |        User Space        |
              |--------------------------|
              |  CLI Tool / Shell Calls  |
              +------------+-------------+
                           |
                           v
              +--------------------------+
              |           VFS            |
              |--------------------------|
              | Mount Table              |
              | Dispatch Logic           |
              | Unified API (`vfs_read`) |
              +------+---------+---------+
                     |         |
        +------------+         +----------------+
        |                                     |
+---------------------+         +--------------------------+
| In-Memory FS Driver |         |   ext2 Driver (Read-Only) |
+---------------------+         +--------------------------+
| RAM Structures      |         |  Block Reading            |
| Path Parser         |         |  Inode/Dir Parsing        |
+---------------------+         +--------------------------+

```