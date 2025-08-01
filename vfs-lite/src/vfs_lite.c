#include <stdio.h>    
#include <stdlib.h> 
#include <string.h>
#include "vfs_lite.h" 

File* fd_table[MAX_OPEN_FILES];
RegisteredDriver driver_table[MAX_DRIVERS];
MountedFileSystem mount_table[MAX_MOUNTED_FILESYSTEMS];

unsigned long driver_count = 0;
unsigned long mount_count = 0;
unsigned long fd_count = 0;


// drivers can call this, and it fills the vfs array of registered drivers in the vfs.h file
// have to call extern func from drivers to send back the mount and unmount functions
void vfs_register_driver(const char* name, unsigned long magic_num, struct FileSystemDriver* fsd) {
    if (driver_count < MAX_DRIVERS) {
        snprintf(driver_table[driver_count].name, MAX_NAME, "%s", name);
        driver_table[driver_count].fsd = fsd;
        driver_table[driver_count].magic_bytes = magic_num;
        driver_count++;
    }
}

// we want to add a MountedFileSystem. So we need the driver, superblock, root_inode
// how do we know what driver!!! Magic num needs to read into data
// so mount should already have the data passed into of the filesystem type
// filesystem can be a blob or a file path to a physical device or disk img based on name
void vfs_mount(char* mount_path, char* filesystem_name, void* filesystem){

    RegisteredDriver* reg_driver = get_driver(filesystem_name);
    if (reg_driver == NULL){
            printf("[-] Driver for this filesystem is not registered yet!\n");
            return;
    }

    if(mount_count < MAX_MOUNTED_FILESYSTEMS){
        // need to make a direntry here and move from driver side
        VFSSuperBlock* super_block = reg_driver->fsd->mount(filesystem);
        HT* table = init_ht();
        ht_set(table, super_block->s_root->name, super_block->s_root);

        for(int i = 0; i < MAX_MOUNTED_FILESYSTEMS; i++){
            if(mount_table[i].root_inode == NULL){
                mount_table[mount_count].driver = reg_driver;
                snprintf(mount_table[mount_count].mount_path, MAX_PATH, "%s", mount_path);
                mount_table[mount_count].root_inode = super_block->s_root->node;
                mount_table[mount_count].super_block = super_block;
                mount_table[mount_count].hash_table = table;
                mount_count++;
                break;
            }
        }
        //ht_print(table);
    }
}

// we need to go through the dirents and build dentry and update a tree that will be
// in the superblock. We use lookup from the driver to get the next inode for us
// and send back that inode which we wrap in a dentry
// we need another function that we can call recursivly for lookups
File* vfs_open(char* path, int flags){
    // call vfs_lookup till I find the right file
    // call the inode open I get back for that file
    MountedFileSystem* curr_filesystem = NULL;
    char* mntpath_ptr = NULL;

    // find fs type
    for(int i = 0; i < mount_count; i++){
        if((mntpath_ptr = strstr(path, mount_table[i].mount_path)) != NULL){
            curr_filesystem = &mount_table[i];
        }
    }
    if(mntpath_ptr == NULL || curr_filesystem == NULL){
        printf("[-] Filesystem is not mounted before trying to open!\n");
        return NULL;
    }
    char path_no_mnt[MAX_NAME];
    // not include the mnt path into the file path I am looking for once we know the fs
    char* file_path_ = mntpath_ptr + strlen(curr_filesystem->mount_path)-1;
    snprintf(&path_no_mnt, MAX_NAME, "%s", file_path_);

    printf("FILE PATH: %s\n", path_no_mnt);

    // we need to assume we have root taken care of at this point
    // so on mount set up the hash table and we would fetch that from there
    // but for now lets just lookup the root node, it should still handle it
    // we should always have the root hashed and setup to fetch
    // inode = hash_get("/") -> inode.lookup(inode, token)
    // realisticaly the lookup would use the inode data offset/block num
    // the use the filepath to strcmp with the names on the dirents in
    // that data block or data blob
    // // here we would call hash_get on the hash token, if its not there add it
    // // and then do a lookup to grab from disk, then after make the cache
    // VFSInode* node = curr_filesystem->root_inode->i_op->lookup(curr_filesystem->root_inode,file_path);

    VFSInode* file_inode = get_inode_of_file(curr_filesystem->root_inode, path_no_mnt, curr_filesystem->hash_table);

    if(NULL == file_inode){
        printf("[-] File doesn't exist!\n");
        return NULL;
    }

    File* fd;
    if(fd_count < MAX_OPEN_FILES){
        char* filename = get_last_token(path_no_mnt, '/');
        fd = file_inode->f_op->open(file_inode,flags,filename);
        if (fd == NULL){
            printf("[-] Failed to open the file, driver open died");
            return NULL;
        }
        // look for empty fd slot
        for(int i = 0; i < MAX_OPEN_FILES; i++){
            if (fd_table[i] == NULL) {
                fd->id = i;
                fd_table[i] = fd;
                fd_count++;
                break;
            }
        }
    }
    else{
        printf("[-] Out of open file descriptors");
        return NULL;
    }

    return fd;
}


// need to add a reset function for the fd file posistion ptr

size_t vfs_read(File* fd, void* buf, size_t size){
    if(fd == NULL){
        printf("[-] File Descriptor is invalid\n");
        return -1;
    }
    if((fd->node->file_size - fd->cursor_postion) < size){ // if file posistion is moved up then different size
        return fd->node->f_op->read(fd, buf, (fd->node->file_size - fd->cursor_postion));
    }
    else{ 
        return fd->node->f_op->read(fd, buf, size);
    }
}

void vfs_unmount(char* mount_path){
    MountedFileSystem* filesystem_to_unmount = NULL;
    char* mntpath_ptr = NULL;
    // find fs type
    for(int i = 0; i < mount_count; i++){
        if((mntpath_ptr = strstr(mount_path, mount_table[i].mount_path)) != NULL){
            filesystem_to_unmount = &mount_table[i];
        }
    }

    if(mntpath_ptr == NULL){
        printf("[-] Filesystem is not mounted before trying to open!\n");
        return NULL;
    }

    filesystem_to_unmount->driver->fsd->unmount(filesystem_to_unmount->super_block);

    ht_destroy(filesystem_to_unmount->hash_table);

    memset(filesystem_to_unmount, 0, sizeof(MountedFileSystem));
    mount_count--;
}


int vfs_close(File** fd){

    int id = (*fd)->id;
    free(*fd);
    fd_table[id] = NULL;
    *fd = NULL; 

    fd_count--;
    return 0;
}

VFSInode* get_inode_of_file(VFSInode* node, char* path, HT* hash_table){
    // we will be direntries everytime we get back an inode
    // when checking the hashtable, we can pull out the closest Inode
    // and search using that inode closet to the end of the path
    char*token = strtok(path, "/");
    VFSInode* tmp_node = node;
    //char* previous_token = NULL;

     while (token != NULL) {
        printf("Token: %s\n", token);

        DirEntry* cache_node = node_from_cache(token, hash_table);

        if (NULL == cache_node){
            tmp_node = tmp_node->i_op->lookup(tmp_node, token);
            if(NULL == tmp_node){
                return NULL;
            }
            if(build_direntry(hash_table, tmp_node, token) < 0){
                printf("[-] Failed to build direntry\n");
                return NULL;
            } 
            //previous_token = token;
        }
        else{
            //previous_token = cache_node->name;
            // we have the cached_node
            printf("Got from cache\n");
            tmp_node = cache_node->node;
            
        }
        token = strtok(NULL, "/");
    }

    //ht_print(hash_table);
    return tmp_node;
}

DirEntry* node_from_cache(char* key, HT* table){
    return (DirEntry*)ht_get(table, key);
}

int build_direntry(HT* table, VFSInode* node, char* filename){
    DirEntry* new_direntry = malloc(sizeof(DirEntry));
    snprintf(new_direntry->name, MAX_NAME, "%s", filename);
    new_direntry->node = node;
    if(!ht_set(table, filename, new_direntry)){
        return -1;
    }
    return 0;
}

RegisteredDriver* get_driver(char* fs_name){
   for(int i = 0; i < driver_count; i ++){
        if((strcmp(driver_table[i].name, fs_name)) == 0){
            return &driver_table[i];
        }
   }
   return NULL;
}

char* get_last_token(char* str, char delimiter) {
    char* last_delimiter_pos = strrchr(str, delimiter);

    if (last_delimiter_pos != NULL) {
        // Delimiter found, return pointer to character after the last delimiter
        return last_delimiter_pos + 1;
    } else {
        // Delimiter not found, the whole string is the last token
        return str;
    }
}

