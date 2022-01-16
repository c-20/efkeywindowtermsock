#if 0
echo="printf \"%s\""
gcc=gcc
src=sock.c
out=sock.e
libs=
uname -a | grep MINGW > /dev/null &&\
  libs=-lWS2_32
$gcc -o $out $src $libs
exit 0
#else

#define HOSTPORT  16660
#define MAXHOOFS  36

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
// #include "fsync.h"


//#ifndef WIN32
//#define WIN32
//#endif
#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif



int main(int argc, char **argv) {
#ifdef WIN32
  WORD versionrequested = MAKEWORD(2, 2);
  WSADATA wsadata;
  int wsa = WSAStartup(versionrequested, &wsadata);
  if (wsa != 0) { printf("WSA fail: %d\n", wsa); return 1; }
  if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
    { printf("WSA version fail\n"); return 1; }
#endif
//  int sockdesc = socket(AF_INET, SOCK_STREAM, 0);
  int sockdesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockdesc == -1) { printf("socket fail\n"); return 2; }
  
#ifdef WIN32
  struct sockaddr_in socke;
  socke.sin_family = AF_INET;
  socke.sin_addr.s_addr = INADDR_ANY;
  socke.sin_port = htons(HOSTPORT);
#else // linux fails above with Address already in use
  struct sockaddr_in socke;
  socke.sin_family = AF_INET;
  socke.sin_addr.s_addr = htonl(INADDR_ANY);
  socke.sin_port = htons(HOSTPORT);
#endif
  
  if (bind(sockdesc, (struct sockaddr *)&socke, sizeof(socke)) < 0) {
    perror("bind fail"); return 3;
  }
  printf("bound...");
  fflush(stdout);

  listen(sockdesc, MAXHOOFS); // 3 ? // wait for a connection

  printf("listening...\n");
  fflush(stdout);
  struct sockaddr_in foote;
  //socklen_t footlen = sizeof(struct sockaddr_in);
  int footlen = sizeof(struct sockaddr_in);
  int readsize = -1;
  while (1) {
    int footdesc = accept(sockdesc, (struct sockaddr *)&foote, &footlen);
    if (footdesc == -1) { printf("accept fail\n"); return 4; }
    char readmessage[2000];
    while ((readsize = recv(footdesc, readmessage, 2000, 0)) > 0) {
      // apparently the bytes aren't null-terminated
      readmessage[readsize] = '\0';
      char messagelen = strlen(readmessage);
      printf("%d bytes received, %d stored\n", readsize, messagelen);
      fflush(stdout);
      //write(footdesc, "[[", 2);
      //write(footdesc, readmessage, strlen(readmessage));
      //write(footdesc, "]]\n", 3);
      char writemessage[2100];
      snprintf(writemessage, 2100, "You said, in %d characters: \n[[ %s ]]\n",
                                  messagelen, readmessage);
      char replylen = strlen(writemessage);
      send(footdesc, writemessage, replylen, 0); //MSG_DONTWAIT);
      //fsync(footdesc);
      printf("some bytes written....\n");
      fflush(stdout);
    }
    close(footdesc);
  }
  if (readsize == 0) {
    printf("----------------\n");
  } else {
    printf("-=-          -=-\n"); 
    printf("-=RECVERR: %2d=-\n", readsize);
    printf("----------------\n");
  }
#ifdef WIN32
  WSACleanup();
#endif
  return 0;
}
#endif

