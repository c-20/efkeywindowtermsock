// todo: a gajillion log-with-param variants
#define LOG(x)     printf(x)

// A for ASCII Char Code .. K for Kernel Code String
#define Kmoveup       "\033[A"
#define Kmovedown     "\033[B"
#define Kmoveleft     "\033[D"
#define Kmoveright    "\033[C"
#define MOVEUP(n)     fprintf(stdout, "\033[%dA", n);
#define MOVEDOWN(n)   fprintf(stdout, "\033[%dB", n);
#define MOVETOSOL     fprintf(stdout, "\n\033[1A");
#define MOVETOEOL     fprintf(stdout, "\n\033[1D");
#define LINEBREAK(w)  fprintf(stdout, "\033[%dD\033[1B", w);
#define MOVERIGHT(n)  fprintf(stdout, "\033[%dC", n);
#define MOVELEFT(n)   fprintf(stdout, "\033[%dD", n);
#define SETCHAR(c)    fprintf(stdout, "%c\033[1D", c);
#define WRITECHAR(c)  fprintf(stdout, "%c", c);
#define LWRITECHAR(c) fprintf(stdout, "%c\033[2D", c);
#define FLUSHBUFFER   LOG0
#define CLEARLINE     fprintf(stdout, "\033[2K");
// http://linux.about.com/library/cmd/blcmdl4_console_codes.htm
// www.systutorials.com/docs/linux/man/4-console_codes/
#define MOVETO(x,y)   fprintf(stdout, "\033[%d;%dH", y, x)
#define MOVETOX(x)    fprintf(stdout, "\033[%dG", x)
#define GOTOCOL(x)    MOVETOX(x)
#define SAVEPOS       fprintf(stdout, "\033[s")
#define RESTOREPOS    fprintf(stdout, "\033[u")

#define OFFC                    "\033[0m"
#define RFGC                    "\033[1;31m"
#define YFGC                    "\033[1;33m"
#define GFGC                    "\033[1;32m"
#define CFGC                    "\033[1;36m"
#define BFGC                    "\033[1;34m"
#define MFGC                    "\033[1;35m"
#define KFGC                    "\033[0;30m"
#define DFGC                    "\033[1;30m"
#define LFGC                    "\033[0;37m"
#define WFGC                    "\033[1;37m"
#define DRFGC                   "\033[0;31m"
#define DYFGC                   "\033[0;33m"
#define DGFGC                   "\033[0;32m"
#define DCFGC                   "\033[0;36m"
#define DBFGC                   "\033[0;34m"
#define DMFGC                   "\033[0;35m"
#define OFGC      DYFGC


#define RXC(ix)         LOG(RFGC); EXTCH(ix); LOG(OFFC);
#define GXC(ix)         LOG(GFGC); EXTCH(ix); LOG(OFFC);
#define BXC(ix)         LOG(BFGC); EXTCH(ix); LOG(OFFC);
#define YXC(ix)         LOG(YFGC); EXTCH(ix); LOG(OFFC);
#define CXC(ix)         LOG(CFGC); EXTCH(ix); LOG(OFFC);
#define MXC(ix)         LOG(MFGC); EXTCH(ix); LOG(OFFC);
#define KXC(ix)         LOG(KFGC); EXTCH(ix); LOG(OFFC);
#define WXC(ix)         LOG(WFGC); EXTCH(ix); LOG(OFFC);
#define INLOG(v1)               YLOG(v1)
#define INNLOG(v1)              INLOG(v1)
#define OUTLOG(v1)              CLOG(v1)
#define ERRLOG(v1)              RLOG(v1)
#define SECTION                 OLOG1("%s)",
#define NULLOUT(thing)
#define SUPPRESS                NULLOUT(


#define RBGC            "\033[41m"
#define REDBG           LOG(RBGC)
#define YBGC            "\033[43m"
#define YELLOWBG        LOG(YBGC)
#define GBGC            "\033[42m"
#define GREENBG         LOG(GBGC)
#define CBGC            "\033[46m"
#define CYANBG          LOG(CBGC)
#define BBGC            "\033[44m"
#define BLUEBG          LOG(BBGC)
#define MBGC            "\033[45m"
#define MAGENTABG       LOG(MBGC)
#define KBGC            "\033[40m"
#define BLACKBG         LOG(KBGC)
#define WBGC            "\033[47m"
#define WHITEBG         LOG(WBGC)
#define NBGC            "\033[0m"
#define RESETBG         LOG(NBGC)
#define CLS             "\033[2J"
#define CLEARSCREEN     LOG(CLS)


#define ENDCOLOUR  "\e[0m"
#define CGREEN     "
