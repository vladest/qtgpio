#include "consolereader.h"
#include <QDebug>
#include <stdio.h>
#include <unistd.h>

/* Initialize new terminal i/o settings */
void ConsoleReader::initTermios(int echo)
{
    Q_UNUSED(echo)
    tcgetattr(0, &oldSettings); /* grab old terminal i/o settings */
    newSettings = oldSettings; /* make new settings same as old settings */
    //newSettings.c_lflag |= (ECHO | ECHOE | ICANON);
    newSettings.c_lflag &= ~(ECHO | ECHOE | ICANON); /* disable buffered i/o */
    tcsetattr(0, TCSANOW, &newSettings); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void ConsoleReader::resetTermios(void)
{
    tcsetattr(0, TCSANOW, &oldSettings);
}

/* Read 1 character without echo */
char ConsoleReader::getch(void)
{
    return getchar();
}

ConsoleReader::ConsoleReader()
{
    initTermios(0);
}

ConsoleReader::~ConsoleReader()
{
    resetTermios();
}

bool inputAvailable()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return (FD_ISSET(0, &fds));
}

void ConsoleReader::run()
{
    while (!isInterruptionRequested()) {
        if (inputAvailable()) {
            char key = getch();
            emit KeyPressed(key);
        }
        msleep(32);
    }
}
