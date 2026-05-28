#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
    #include <direct.h>
    #include <conio.h>
    #define MKD(p) _mkdir(p)
    #define GWD _getcwd
    #define CD _chdir
#else
    #include <unistd.h>
    #include <termios.h>
    #define MKD(p) mkdir(p, 0777)
    #define GWD getcwd
    #define CD chdir
    
    static inline int _getch(void) {
        struct termios o, n;
        int c;
        tcgetattr(0, &o);
        n = o;
        n.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(0, TCSANOW, &n);
        c = getchar();
        tcsetattr(0, TCSANOW, &o);
        return c;
    }
#endif

#endif /* PLATFORM_H */
