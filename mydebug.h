#define _STR(x)      #x
#define _STR2(x)     _STR(x)
#define __SLINE__    _STR2(__LINE__)
#define HERE         __FILE__ "(" __SLINE__ ")"

#ifdef DEBUG
#define dprintf printf
#else
#define dprintf 1 ? (void) 0 : printf
#endif

#define dphere() dprintf(HERE "\n")
