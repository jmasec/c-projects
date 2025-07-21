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
```
+--------------------+   0x0000
| Superblock         |
+--------------------+

+--------------------+   0x0040
| Inode Table        |--> [inode 0]  (DIR: "/")
|                    |    [inode 1]  (DIR: "/docs")
|                    |    [inode 2]  (FILE: "/hello.txt")
|                    |    [inode 3]  (FILE: "/docs/readme.md")
+--------------------+

+--------------------+   0x0100
| Dirents for inode 0 ("/")         |
|  - "hello.txt" --> inode 2        |
|  - "docs"      --> inode 1        |
+--------------------+

+--------------------+   0x0180
| Dirents for inode 1 ("/docs")     |
|  - "readme.md" --> inode 3        |
+--------------------+

+--------------------+   0x0200
| File Data for inode 2 (hello.txt) |
|  "Hello, world!"                  |
+--------------------+

+--------------------+   0x0300
| File Data for inode 3 (readme.md) |
|  "# Documentation"               |
+--------------------+
```
```
vfs_open("/docs/readme.txt")
  │
  └── path split → ["docs", "readme.txt"]
        │
        └── VFS starts at root inode
              ↓
        lookup("docs") via root_inode->fops->lookup()
              ↓
        lookup("readme.txt") via docs_inode->fops->lookup()
              ↓
        open() via readme_inode->fops->open()
              ↓
        return fd
```

You call VFS mount:
vfs_mount("/mnt/cramfs", "cramfs", blob);
VFS looks up the driver by name from driver_table

Gets the corresponding FileSystemDriver*

VFS calls driver’s .mount(blob) function

The driver parses the blob and returns the root inode

VFS adds this to its mount_table

Stores: mount path (/mnt/cramfs), driver pointer, and root inode

Updates I need to do: 
Move from the data blob I have to blocks of memory to be more realistic

- I am gonna have the drivers use the structs from the VFS kernel side

int fd = open("/home/bob/file.txt", O_RDONLY);

VFS Path Resolution Tree:
(superblock)
   |
   v
[dentry: "/"] --> [inode: dir]
      |
   [dentry: "home"] --> [inode: dir]
      |
   [dentry: "bob"] --> [inode: dir]
      |
   [dentry: "file.txt"] --> [inode: file]

