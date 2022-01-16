#if 0
gcc=gcc
src=hoof.c
out=hoof.e
libs=
uname -a | grep MINGW > /dev/null &&\
  libs=-lWS2_32
$gcc -o $out $src $libs
exit 0
#else
#define HOSTNODE   "192.168.20.21"
//#define HOSTNODE   "127.0.0.1"
#define HOSTPORT   16660

#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

// #define msg(txt)  printf(txt "\n")

#define MESSAGE(txt) puts(txt "\n"); fflush(stdout)
#define MSG(txt)     MESSAGE(txt)
#define PROMPT(txt)  puts(txt); fflush(stdout)
#define ANSWER(txt)  puts(txt); fflush(stdout)
// todo: colour markup
// these flushes are needed in Windows but not linux

#define RT return

#include "AWS.h"


#define SOCKP    struct sockaddr *
//#ifdef WIN32
//#define SOCKIN   sockaddr_in
//#else
#define SOCKIN   struct sockaddr_in
//#endif

// aws stands for autowinsock
// AWSinit() creates vars AWSversion, AWSdata and AWSerror

int main(int argc, char **argv) {
  AWSinit();
  int sockdesc = AWSopen(); //socket(AF_INET, SOCK_STREAM, 0);
  if (sockdesc == -1) { AWSerror("socket fail"); RT 2; }

  SOCKIN socke;
  socke.sin_family = AF_INET;
  //socke.sin_addr.s_addr = INADDR_ANY;
  socke.sin_addr.s_addr = inet_addr(HOSTNODE);
  socke.sin_port = htons(HOSTPORT);

  if (connect(sockdesc, (SOCKP)&socke, sizeof(socke)) < 0)
    { AWSclose(sockdesc); AWSerror("connect fail"); RT 3; }

  while (1) {
    char sendmsg[1000];
    PROMPT("SEND: ");
    fflush(stdout);

    scanf(" %[^\n]", sendmsg);
    if (send(sockdesc, sendmsg, strlen(sendmsg), 0) < 0)
      { AWSclose(sockdesc); AWSerror("send fail"); RT 4; }

// close socket before or after WSAcleanup()?!?!?!?!?!
    // or will closing a broken socket cause a lock?!?!?
    MSG("waiting for reply...\n");

    int readsize = -1;
    char recvmsg[2000];
    if ((readsize = recv(sockdesc, recvmsg, 2000, 0)) < 0)
      { AWSclose(sockdesc); AWSerror("recv fail"); RT 5; }
    else {
      recvmsg[readsize] = '\0';
      ANSWER("RECV: ");
      puts(recvmsg);
      fflush(stdout);
    }
    // reply includes linebreak
    // todo: close_sock on recv fail or send fail?
  }
//  closesocket(sockdesc);
  AWSclose(sockdesc);
  AWSstop();
  return 0;
}


#endif

