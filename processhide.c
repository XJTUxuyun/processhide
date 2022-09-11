#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include "../guardfs/ioctl.h"

static int get_dir_name(DIR* dirp, char* buf, size_t size)
{
    int fd = dirfd(dirp);
    if(fd == -1)
    {
        return 0;
    }

    char tmp[64];
    snprintf(tmp, sizeof(tmp), "/proc/self/fd/%d", fd);

    ssize_t ret = readlink(tmp, buf, size);
    if(ret == -1)
    {
        return 0;
    }

    buf[ret] = 0;
    return 1;
}

static int guard_process_protect_test(int pid)
{
    int fd = open("/dev/guardfs", O_RDWR);
    if (fd > 0)
    {
        int r = ioctl(fd, GUARD_PROCESS_PROTECT_TEST, pid);
        if (r == -1 && errno == EACCES)
        {
            close(fd);
            return 1;
        }
        close(fd);
    }

    return 0;
}

struct dirent *(*org_readdir)(DIR *dirp) = NULL;
struct dirent *readdir(DIR *dirp)
{
    if (!org_readdir)
    {
        org_readdir = dlsym(RTLD_NEXT, "readdir");
    }

    struct dirent* dir;
    while (1)
    {
        dir = org_readdir(dirp);
        if (dir)
        {
            char dir_name_buf[4096] = {0};
            if (get_dir_name(dirp, dir_name_buf, sizeof(dir_name_buf)))
            {
                if (!strncmp(dir_name_buf, "/proc", 4096))
                {
                    int pid = 0;
                    if (sscanf(dir->d_name, "%d", &pid) == 1)
                    {
                        if (guard_process_protect_test(pid))
                        {
                            continue;
                        }
                    }
                }
            }
        }

        break;
    }

    return dir;
}

struct dirent64 *(*org_readdir64)(DIR *dirp) = NULL;
struct dirent64 *readdir64(DIR *dirp)
{
    if (!org_readdir64)
    {
        org_readdir64 = dlsym(RTLD_NEXT, "readdir64");
    }

    struct dirent64* dir;
    while (1)
    {
        dir = org_readdir64(dirp);
        if (dir)
        {
            char dir_name_buf[4096] = {0};
            if (get_dir_name(dirp, dir_name_buf, sizeof(dir_name_buf)))
            {
                if (!strncmp(dir_name_buf, "/proc", 4096))
                {
                    int pid = 0;
                    if (sscanf(dir->d_name, "%d", &pid) == 1)
                    {
                        if (guard_process_protect_test(pid))
                        {
                            continue;
                        }
                    }
                }
            }
        }

        break;
    }

    return dir;
}


