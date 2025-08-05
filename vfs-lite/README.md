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

```
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

Start at root inode (0)
→ type = DIR, direct[0] = 20

Read block 20 (dirents):

See "docs" → inode 2

Inode 2 → type = DIR, direct[0] = 21

Read block 21 (dirents):

See "readme.md" → inode 3

Inode 3 → type = FILE, direct[0] = 31
→ read block 31 → "# Documentation"
```

```
Block 0: Superblock

Magic number: 0x4D465346

Block size: 512

Total blocks: 9

Inode table start: block 3

Data block start: block 5

Root inode: 0

Block 1: Block bitmap (9 blocks used)

Block 2: Inode bitmap (4 inodes used)

Blocks 3-4: Inode table

Inode 0: root directory /

Inode 1: hello.txt (file, size=13 bytes)

Inode 2: docs/ (directory)

Inode 3: docs/readme.md (file, size=15 bytes)

Block 5: Directory entries for /

hello.txt → inode 1

docs → inode 2

Block 6: Directory entries for /docs

readme.md → inode 3

Block 7: "Hello, world!" (hello.txt data)

Block 8: "# Documentation" (readme.md data)
```

```
Block size: 512 bytes

Total blocks: 20

Used blocks: first 9 blocks are populated (same layout as before)

Free space: 11 empty blocks available (blocks 9–19)
```