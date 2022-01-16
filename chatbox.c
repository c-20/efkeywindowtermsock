#if 0
gcc=gcc
src=chatbox.c
out=chatbox.e
$gcc -o $out $src
return
# exit 0 <-- exits shell
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define FF   fflush(stdout);
//#define ESC  "\e"
#define CSI  "\e["

//#include "initshift.h"
// ^ this integration didn't happen ?

#define BOXLR 0
#define BOXUD 1

#define BOXTL 2
#define BOXTR 3
#define BOXBL 4
#define BOXBR 5

#define MAXBUFFERLEN   4096
// TODO: CHECK LLIMIT !!!!

typedef struct _Window {
  int x, y, w, h;
  int ix;
} Window;

typedef struct _Box {
  int x, y, w, h;
  int r, c;
  char title[64];
  char *border;
  int spinval, spinlen;
  char spinborder[4][16];
  char buffer[MAXBUFFERLEN];
  int bufferpos;
  struct _Box *next;
  struct _Box *prev;
  struct _Box *sub;
} Box;
Box newbox(int x, int y, int w, int h, char *title) {
  Box b = { x: x, y: y, w: w, h: h, r: 1, c: 1 };
  b.spinlen = 4; //    \\/    |-   /\\   -|
  snprintf(b.spinborder[0], 16, "-|/\\-|");  //  \\/  ");
  snprintf(b.spinborder[1], 16, "-|-|\\/");  //  |-   ");
  snprintf(b.spinborder[2], 16, "-|\\/|-");  //  /\\  ");
  snprintf(b.spinborder[3], 16, "-||-/\\");  //  -|   ");
  b.border = b.spinborder[b.spinval = 0];
  snprintf(b.title, 64, "%s", title);
  b.buffer[0] = '\0';
  b.bufferpos = 0;
  b.prev = b.next = b.sub = NULL;
  return b;
}

void savecursor() {
  printf(CSI "s");
}

void restorecursor() {
  printf(CSI "u");
}

void cursorgoto(int row, int col) {
// rows and cols start at 1, not 0 ....
///??do thhey?
  printf(CSI "%d;%dH", row + 1, col + 1);
}


void writech(char ch) {
//  fputc(ch, stdout);
  printf("%c", ch);
  FF
}

void gotobox(Box *b, int localtop) {
  int row = b->y + b->r + localtop;
  int col = b->x + b->c;
  cursorgoto(row, col);
}

void blankline(Box *b, int localtop, int row) {
  // blanking line 0 ought to only blank the title region
  // blanking lines in box won't eat borders
  // blanking lines above or below box should be full width of box
  savecursor();
  int x = 1; // b->x offset added later
  int xlim = b->w - 2; // keep border
  if (row == 0) { // blank title
    int btitlelen = strlen(b->title);
    int bcentre = b->w / 2;
    int bcentrel = bcentre - (btitlelen / 2);
    x = bcentrel;
    xlim = bcentrel + btitlelen - 1;
  } else if (row < 0) { x = 0; xlim = b->w; }
  else if (row == b->h) { // redraw box ... it spins ....

  } else if (row > b->h) { x = 0; xlim = b->w; }
  int y = row;
  cursorgoto(b->y + y + localtop, b->x + x);
  while (x <= xlim) {
    writech(' ');
    x++;
  }
}
//void redrawbox(Box *b) { // consider x y w h offsets?  // , int localtop) {
void spinbox(Box *b) {
  b->spinval = (b->spinval + 1) % b->spinlen;
  b->border = b->spinborder[b->spinval];
}
//}

void drawbox(Box *b, int localtopv) {
  savecursor();
//  b->spinval = 0;
//  b->border = b->spinborder[b->spinval];
  int btitlelen = strlen(b->title);
  int bcentre = b->w / 2;
  int bcentrel = bcentre - (btitlelen / 2);
  int bcentrer = bcentrel + btitlelen - 1;
  int y = 0;
  while (y < b->h) {
    int x = 0;
    cursorgoto(b->y + y + localtopv, b->x + x);
    while (x < b->w) {
      if      (y == 0        && x == 0       ) { writech(b->border[BOXTL]); }
      else if (y == 0        && x == b->w - 1) { writech(b->border[BOXTR]); }
      else if (y == 0 && x == bcentrel - 1   ) { writech(' '); }
      else if (y == 0 && x >= bcentrel && x <= bcentrer) { writech(b->title[x - bcentrel]); }
      else if (y == 0 && x == bcentrer + 1   ) { writech(' '); }
      else if (y == 0                        ) { writech(b->border[BOXLR]); }
      else if (y == b->h - 1 && x == 0       ) { writech(b->border[BOXBL]); }
      else if (y == b->h - 1 && x == b->w - 1) { writech(b->border[BOXBR]); }
      else if (y == b->h - 1                 ) { writech(b->border[BOXLR]); }
      else if (                 x == 0       ) { writech(b->border[BOXUD]); }
      else if (                 x == b->w - 1) { writech(b->border[BOXUD]); }
//      else if (
      else { writech(' '); } // clear the box
      x++;
    }
    y++;
  }
  restorecursor();
}


// HOW TO WRAP TEXT OMG


//void drawtextline(Box *b, char *buf, char *bufstop) {
  // goto current r,c and leave cursor at next line??
// consider padding as is own variable?
char *gettextline(Box *b, char *buf, char **start, char **stop) {
  if (*buf == '\0') { *start = NULL; *stop = NULL; return buf; }
  int prevspace = (*buf ==  ' ') ? 1 : 0;
  int prevlineb = (*buf == '\n') ? 1 : 0;
// prevspace wil be the last space in the line
// etra spaces at the end of a line could pad the next one
// but if the wrapping moves so do they. so EOL pad, extras overflow, one space eaten
  if (prevspace) { buf++; } // or set hanging indent
  if (prevlineb) { buf++; } // or keep for double-line-spacing
  char *bufstart = buf; // if first char is \0 then start returned as NULL
  char *bufend = buf;   // if first char is \0 then stop returned as NULL
  int bufseek = b->w - 2;  // exclude border columns !
  while (--bufseek >= 0) { // stop at EOL or EObuf
    if (*bufend == '\0' || *bufend == '\n') { break; }
    else { bufend++; }
  } // only increment until end of buffer is reached
  buf = bufend; // update return pointer (at '\n' or ' ')
  int shortline = 0; // line with "1" has same start and stop
  if (*bufend == '\0' || *bufend == '\n') { bufend--; shortline = 1; }
  // if prevspace, this is a continuation of a line .. unless it is a padded line after a linebreak
  int maxwraplen = b->w / 3; // don't wrap words wider than a third of the line
  char *wrapend = bufend;
  if (!shortline && *wrapend != ' ') { // only wrap if at edge of box
    char *wraplim = wrapend - maxwraplen; // if < bufstart, loop breaks anyway
    while (*wrapend != ' ') { // find the last space available for word wrap
      if (wrapend <= bufstart || wrapend <= wraplim) { break; }
      else { wrapend--; } // stop if first char reached or over max wrap width
    }
    if (*wrapend == ' ') // if a space wasn't found, bufend stays where it was
      { bufend = wrapend - 1; buf = wrapend; } // else wrap snips buf early
  } // if shortlines wrap, last word ends up on the next row
  *start = bufstart;
  *stop = bufend;
  return buf; // at space if wrapped, at char after stop if not
}


// from ROW to ROW not LINE to LINE (some lines need multiple rows)
//void drawtextlinerange(Box *b, int from, int until) {
void nexttextrow(Box *b) { // return false if no

}

void prevtextrow(Box *b) { // return false if no

}

int drawtextrow(Box *b, int localtopv, int line, char *start, char *stop) {
  if (line > (b->h - 2)) { return 0; } // not visible
  int consolerow = b->y + localtopv + line;
  int consolecol = b->x + 1; // after border col
  savecursor();
  cursorgoto(consolerow, consolecol);
  char *thisch = start;
  int charcount = 1;
  while (charcount <= (b->w - 2)) {
    if (thisch > stop) { writech(' '); }
    else { writech(*thisch); thisch++; }
    charcount++;
  }
  restorecursor();
  return 1; // did draw
}

void drawtextrows(Box *b, int localtopv, int firstrow) { // moves cursor
  char *buf = b->buffer;
  char *start = buf;
  char *stop = buf;
  char *prevstart = NULL; // start;
  char *prevstop = NULL; // stop;
  int line = 1;
  int row = 1;
  while (*buf != '\0') {
    buf = gettextline(b, buf, &start, &stop);
    if (row >= firstrow) { // write visible rows until declined
      if (!drawtextrow(b, localtopv, row, start, stop)) { break; }
    } // then update row counter
    if      (*buf == ' ')  { row++; }         // wrapped line
    else if (*buf == '\n') { line++; row++; } // linebreaked line
    else                   { row++; }         // unwrappable line
    prevstart = start;
    prevstop = stop;
  }
  int cursorx = (prevstop == NULL) ? 1 : (1 + prevstop - prevstart + 1);
  int cursory = (prevstop == NULL) ? 1 : (1 + row - firstrow - 1);
  cursorgoto(b->y + localtopv + cursory, b->x + cursorx);
}


void drawboxes(Box *b, int localtopv, char ch) { // need to know if resize event
  spinbox(b); // spin outer box
  if (ch == 'B' || ch == 'C' || ch == 'D') { // down, right, left
    blankline(b, -1, localtopv); // need above-line blanking
    if (ch == 'C' || ch == 'D')  // two lines for left/right due to wrap
      { blankline(b, -2, localtopv); }
  }
  while (b) {
    while (b->sub) { // more than one sub level nay not work
      Box *c = b->sub;
      while (c) {
        drawbox(c, localtopv);
// currently not drawing sub-box text
        c = c->next;
      }
    }
    drawbox(b, localtopv);
    int firstlinev = 1;
    drawtextrows(b, localtopv, firstlinev); // cursor goes to end of text
    b = b->next;
  } // draw all top-level boxes -- sub-boxes will need to recurse
}


// ------------------------------------------------


void writebufferch(Box *b, char ch) {
  if (ch == '\b') {
    if (b->bufferpos > 0) { --(b->bufferpos); }
    b->buffer[b->bufferpos] = '\0';
  } else {
    b->buffer[b->bufferpos] = ch;
    b->buffer[++(b->bufferpos)] = '\0';
  }
}

void resizewindow(int lines, int cols) {
  printf("\e[8;%d;%dt", lines, cols);
}

void print2i(int row, int col, const char *format, int p1, int p2) {
  savecursor();
  cursorgoto(row, col);
  printf(format, p1, p2);
  restorecursor(); FF
}
void print0i(int row, int col, const char *literal) {
  savecursor();
  cursorgoto(row, col);
  printf("%s", literal); // expecting no format
  restorecursor(); FF
}

int digitcount(int number) {
  int digits = 0;
  while (number > 0)
    { digits++; number /= 10; }
  return digits;
}

int main(int argc, char **argv) {
  int lines = 0;
  int cols = 0;
  FILE *fsize = popen("stty size", "r");
  if (!fsize) { printf("no fsize\n"); return 1; }
  char cs = 'l';
  int ch = '\0';
  while (ch == ch) { // != 'R') {
    ch = fgetc(fsize); //getchar();
    if (ch == EOF) { printf("EOF\n"); break; }
    else if (ch == 10) { break; } // stop on \n
    else if (ch >= '0' && ch <= '9') {
      if (cs == 'l') { lines = lines * 10 + ch - '0'; }
      if (cs == 'c') { cols  =  cols * 10 + ch - '0'; }
    } else if (ch == ' ') {
      if (cs == 'l') { cs = 'c'; }
    } else {
      printf("ch: %d\n", ch);
    }
  }
  FF // im case of logged evemts
  pclose(fsize);
  // ok now read keys ....
  system("stty raw -echo");
  char escch = '\0';
  char actch = 'S'; // draw box immediately // '\0';
  int number = 0;
  int digits = 0;
  int shiftstate = 0;
  initshiftstatemachine();
//getshiftstate();
  // down adds a line if at the bottom line
  int localboxlines = 5;
  Box local = newbox(0, 0, cols, localboxlines + 2, "CHAT");
  local.r = localboxlines; // start on last line
// local position is ......................
// wherererererererere can also select a sub-box if in the right spot !
//  int localtop = lines - local.h; // needs recalc if either var change !
#define localtop   (lines - local.h)
  gotobox(&local, localtop);

  ch = '\0';
  do {
    shiftstate = getshiftstate();
//  while (1) {
//    ch = getchar();
    if    (ch == EOF) { break; } // should break on ctrl+z
    else if (ch == 0) {        } // ignore NUL char -- used for draw on init
    else if (ch == 3) { break; } // should break on ctrl+c
    else if (ch == '\x1b') {
      //printf("1B\n"); FF
      escch = ch;
    } else if (escch == 27) {
      if (ch == '[') { escch = '['; }
      else { escch = '\0'; }
    } else if (escch == '[') {
      if (ch == 'A') { actch = 'y'; escch = '\0'; } // UP
      if (ch == 'B') { actch = 'Y'; escch = '\0'; } // DOWN
      if (ch == 'C') { actch = 'X'; escch = '\0'; } // RIGHT
      if (ch == 'D') { actch = 'x'; escch = '\0'; } // LEFT
    } else if (ch == '\r') { // enter is carriage return (linux)
//      printf("\n");
//      local.h++; // UPSIZE WITH SHIFT+ENTER ???
//      actch = 'S';
      writebufferch(&local, '\n');
//printf("ENTER: %d\n", ch);
//    } else if (ch == '\b' && local.x == 0) {
    } else if (ch == 127) { // backspace maps to 127 not \b
      writebufferch(&local, '\b');
//      printf("\b"); // or move up + scroll up if this doesn't work
//      local.h--;
//      actch = 'S';
      // DOWNSIZE WITH SHIFT+BACKSPACE????
    } else if (ch >= ' ' && ch <= '~') { // all keys in this range are valid ?
      // how to test if shift is pressed .... ?
      writebufferch(&local, ch);

//    } else if (ch >= '0' && ch <= '9') {
    } else if (ch == '0' && ((digits == 0 && shiftstate == 1) || (digits > 0 && number == 0))) {
      // a (shift)000 1(shift)08 will be the same as 108
      digits++; // only consecutive prepended zeros with first shifted are detected here
      if (number == 0 && ch == '0') { digits++; }
      else { number = (number * 10) + (ch - '0'); }
    } else if (digits > 0 && ch >= '0' && ch <= '9') { // shiftstate ignored if not caught above
      number = (number * 10) + (ch - '0');
      if (digitcount(number) >= digits) {
        int sendnumber = number;
        digits = number = 0;
        gotonumber(sendnumber);
        // unless callmode > 0, then assign the number to the call ?
      }
    } else if (ch == '`') {
      callmode = 3; // 3-letter-mnemonic mode
      callchars = 0;
    } else if (callmode > 0) {
      callsig[callchars] = ch;
      if (++callchars >= callmode) {
        // run callsign action ---- consider disambiguation at this point for LSX-ls and LSX-ls --color=yes, set default - unset default -- can shift+0 mean nextparam<-- ?
        callsig[callchars] = '\0';
// check if window exists and create if it doesn't ? or just make it appear ....
        Window sigwin = { 2, 2, 10, 3, 9, "SIGNAL" };
        writewindow(sigwin, callsig, CGREEN);
      } else {
        callsig[callchars] = '\0';
        Window sigwin = { 2, 2, 10, 3, 9, "SIGGGG" };
        writewindow(sigwin, callsig, CYELLOW);
      }
    } else {
      printf("KEYPRESS %2x (%d)\n", ch, ch); FF usleep(500000); // 500ms wait
    }
    //if (actch == 'y') { lines--; actch = 'S'; } // printf("\e[B"); FF actch = 'S'; }
// seems to be backwards ......
#if 0
    if (actch == 'y') { lines++; printf("\e[A"); FF actch = 'S'; }
    if (actch == 'Y') { lines--; printf("\e[B"); FF actch = 'S'; } // write line above when redrawing box
    if (actch == 'x') { cols--;  printf("\e[C"); FF actch = 'S'; }
    if (actch == 'X') { cols++;  printf("\e[D"); FF actch = 'S'; }
#else
// now backwards seems to be backwards...
//printf("\e[A"); FF }
    if (actch == 'y') { lines--; actch = 'S'; }
    if (actch == 'Y') { lines++; actch = 'S'; }
//printf("\e[B"); FF actch = 'S'; }
    if (actch == 'x') { cols--;  local.w--; actch = 'S'; }
//printf("\e[C"); FF actch = 'S'; }
    if (actch == 'X') { cols++;  local.w++; actch = 'S'; }
//printf("\e[D"); FF actch = 'S'; }
#endif
// only correct it in windows?
    // in the case of --down--
    // draw boxes, then draw sizebox overlay
    // need to process resize to correctly draw boxes else COLSKEW

void erasewindow(Window w) {


}
void writewindow(Window w, char *text, char *prefix) {
  
  if (prefix) { printf(ENDCOLOUR); }
}

    if (actch == 'S') {
      actch = '\0';
      // erase existing size-write block area (one extra in case of wrap)
      print0i(1, cols - 12, "              "); // -1 to left, +2 wide
      print0i(2, cols - 12, "              ");
      print0i(3, cols - 12, "              ");
      // then resize and write new size-block area
      resizewindow(lines, cols);
      // draw boxes then overlay for z-order
      drawboxes(&local, localtop, ch);
      print2i(1, cols - 11, "[%3d x %3d]", lines,  cols  );
      print2i(2, cols - 11, "(%3d / %3d)", local.w, local.h);
      print2i(3, cols - 11, "{%3d , %3d}", local.x, local.y);
    } else { // non-resize-event activity
      drawboxes(&local, localtop, '\0'); // no need to pass ch
    }



//    gotobox(&local, localtop); // lines - local.h);

    ch = getchar(); // get char after first iteration
    // but should be non-blocking read ...?
  } while (1);
  FF
  system("stty -raw echo");
  return 0;
}

#endif

