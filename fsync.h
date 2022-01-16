//#include <config.h>
#include <unistd.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <errno.h>
#include <io.h>

int fsync(int fd) {
  HANDLE h = (HANDLE)_get_osfhandle(fd);
  if (h == INVALID_HANDLE_VALUE)
    { errno = EBADF; return -1; }
  if (!FlushFileBuffers(h)) {
    DWORD err = GetLastError();
    if (err == ERROR_ACCESS_DENIED) {
      return 0;
    } else if (err == ERROR_INVALID_HANDLE) {
             errno = EINVAL; return -1;
    } else { errno = EIO;    return -1; }
  }
  return 0;
}

