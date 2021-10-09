// chfs client.  implements FS operations using extent and lock server
#include "chfs_client.h"
#include "extent_client.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

chfs_client::chfs_client()
{
  ec = new extent_client();
}

chfs_client::chfs_client(std::string extent_dst, std::string lock_dst)
{
  ec = new extent_client();
  if (ec->put(1, "") != extent_protocol::OK)
    printf("error init root dir\n"); // XYB: init root dir
}

chfs_client::inum
chfs_client::n2i(std::string n)
{
  std::istringstream ist(n);
  unsigned long long finum;
  ist >> finum;
  return finum;
}

std::string
chfs_client::filename(inum inum)
{
  std::ostringstream ost;
  ost << inum;
  return ost.str();
}

bool chfs_client::isfile(inum inum)
{
  extent_protocol::attr a;

  if (ec->getattr(inum, a) != extent_protocol::OK)
  {
    printf("error getting attr\n");
    return false;
  }

  if (a.type == extent_protocol::T_FILE)
  {
    printf("isfile: %lld is a file\n", inum);
    return true;
  }
  printf("isfile: %lld is a dir\n", inum);
  return false;
}
/** Your code here for Lab...
 * You may need to add routines such as
 * readlink, issymlink here to implement symbolic link.
 * 
 * */

bool chfs_client::isdir(inum inum)
{
  // Oops! is this still correct when you implement symlink?
  return !isfile(inum);
}

int chfs_client::getfile(inum inum, fileinfo &fin)
{
  int r = OK;

  printf("getfile %016llx\n", inum);
  extent_protocol::attr a;
  if (ec->getattr(inum, a) != extent_protocol::OK)
  {
    r = IOERR;
    goto release;
  }

  fin.atime = a.atime;
  fin.mtime = a.mtime;
  fin.ctime = a.ctime;
  fin.size = a.size;
  printf("getfile %016llx -> sz %llu\n", inum, fin.size);

release:
  return r;
}

int chfs_client::getdir(inum inum, dirinfo &din)
{
  int r = OK;

  printf("getdir %016llx\n", inum);
  extent_protocol::attr a;
  if (ec->getattr(inum, a) != extent_protocol::OK)
  {
    r = IOERR;
    goto release;
  }
  din.atime = a.atime;
  din.mtime = a.mtime;
  din.ctime = a.ctime;

release:
  return r;
}

#define EXT_RPC(xx)                                          \
  do                                                         \
  {                                                          \
    if ((xx) != extent_protocol::OK)                         \
    {                                                        \
      printf("EXT_RPC Error: %s:%d \n", __FILE__, __LINE__); \
      r = IOERR;                                             \
      goto release;                                          \
    }                                                        \
  } while (0)

// Only support set size of attr
int chfs_client::setattr(inum ino, size_t size)
{
  int r = OK;

  /*
     * your code goes here.
     * note: get the content of inode ino, and modify its content
     * according to the size (<, =, or >) content length.
     */






  return r;
}

// - If a file named @name already exists in @parent, return EXIST.
// - Pick an ino (with type of chfs_client::inum) for file @name.
//   Make sure ino indicates a file, not a directory!
// - Create an empty extent for ino.
// - Add a <name, ino> entry into @parent.
// - Change the parent's mtime and ctime to the current time/date
//   (this may fall naturally out of your extent server code).
// - On success, store the inum of newly created file into @e->ino,
//   and the new file's attribute into @e->attr. Get the file's
//   attributes with getattr().
//
// @return chfs_client::OK on success, and EXIST if @name already exists.

int chfs_client::createFileOrDir(inum parent, const char *name, mode_t mode, inum &ino_out, int type)
{
  int r = OK;
  bool found = false;
  inum tmp;                         // not necessary so why?
  lookup(parent, name, found, tmp); // ?need to find out after implement findout
  if (found)
    return EXIST;
  // pick an inum
  ec->create(type, ino_out);
  // get the exist content in the parent directory
  // then append one to it. it same for directory of file.
  std::string buf;
  ec->get(parent, buf);
  buf.append(std::string(name) + ":" + filename(ino_out) + "/");
  ec->put(parent, buf);
  return r;
}

int chfs_client::create(inum parent, const char *name, mode_t mode, inum &ino_out)
{
  int r = OK;

  /*
     * your code goes here.
     * note: lookup is what you need to check if file exist;
     * after create file or dir, you must remember to modify the parent infomation.
     */
  r = createFileOrDir(parent, name, mode, ino_out, extent_protocol::T_FILE);

  return r;
}

int chfs_client::mkdir(inum parent, const char *name, mode_t mode, inum &ino_out)
{
  int r = OK;

  /*
     * your code goes here.
     * note: lookup is what you need to check if directory exist;
     * after create file or dir, you must remember to modify the parent infomation.
     */
  r = createFileOrDir(parent, name, mode, ino_out, extent_protocol::T_DIR);

  return r;
}

int chfs_client::lookup(inum parent, const char *name, bool &found, inum &ino_out)
{
  int r = OK;
  printf("lookup parent: %llu, name: %s\n", parent, name);
  std::list<dirent> list;
  readdir(parent, list);
  for (std::list<dirent>::const_iterator it = list.begin(); it != list.end(); ++it)
  {
     if (it->name.compare(name) == 0) {
      found = true;
      ino_out = it->inum;
      return r;
    }
    found = false;
    return r;
  }

  /*
     * your code goes here.
     * note: lookup file from parent dir according to name;
     * you should design the format of directory content.
     */

  return r;
}

int chfs_client::readdir(inum dir, std::list<dirent> &list)
{
  int r = OK;

    /*
     * your code goes here.
     * note: you should parse the dirctory content using your defined format,
     * and push the dirents to the list.
     */
    // my format of dir content: "name:inum/name:inum/name:inum/"

    // get directory content
    std::string buf;
    ec->get(dir, buf);

    // traverse directory content
    int name_start = 0;
    int name_end = buf.find(':');
    while (name_end != std::string::npos) {
        std::string name = buf.substr(name_start, name_end - name_start);
        int inum_start = name_end + 1;
        int inum_end = buf.find('/', inum_start);
        std::string inum = buf.substr(inum_start, inum_end - inum_start);
            
        struct dirent entry;
        entry.name = name;
        entry.inum = n2i(inum);

        list.push_back(entry);
        
        name_start = inum_end + 1;
        name_end = buf.find(':', name_start);
    }

    return r;
}

int chfs_client::write(inum ino, size_t size, off_t off, const char *data,
                       size_t &bytes_written)
{
  int r = OK;

  /*
     * your code goes here.
     * note: write using ec->put().
     * when off > length of original file, fill the holes with '\0'.
     */

  return r;
}

int chfs_client::unlink(inum parent, const char *name)
{
  int r = OK;

  /*
     * your code goes here.
     * note: you should remove the file using ec->remove,
     * and update the parent directory content.
     */

  return r;
}