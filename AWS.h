#ifdef WIN32
#define AWSinit() \
  WORD AWSversion = MAKEWORD(2, 2); WSADATA AWSdata;                       \
  int AWSerrno = WSAStartup(AWSversion, &AWSdata);                         \
  if (AWSerrno != 0) { AWSerror("WSAStartup failed"); RT 1; }              \
  else if (LOBYTE(WSAdata.wVersion) != 2 || HIBYTE(WSAdata.wVersion) != 2) \
    { AWSnote("WSAversion mismatch !"); }
#define AWSerror(msg)   { perror(msg); WSAcleanup(); }
#define AWSstop()       { WSAcleanup(); }
#define AWSnote(msg)    { printf("NOTE: %s", msg); }
#define AWSclose(soc)   { closesocket(soc); }
#else
#define AWSinit()
#define AWSerror(msg)   { perror(msg); }
#define AWSstop()       { }
#define AWSnote(msg)    { printf("NOTE: %s", msg); }
#define AWSclose(soc)   { close(soc); }
#endif // still need to RT after AWSerror!
//#define AWSopen()       socket(AF_INET, SOCK_STREAM, 0)
//#ifdef WUT
#define AWSopen()	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
//#define AWSopen()	socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)
//#endif
