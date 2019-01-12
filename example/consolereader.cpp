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
    newSettings.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(0, TCSANOW, &newSettings); /* use these new terminal i/o settings now */
    setbuf(stdin, NULL);
}

/* Restore old terminal i/o settings */
void ConsoleReader::resetTermios(void)
{
    tcsetattr(0, TCSANOW, &oldSettings);
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
    tv.tv_usec = 32000;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    int ret;
    if (select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) > 0) {
        return (FD_ISSET(0, &fds));
    }
    return false;
}

void ConsoleReader::run()
{
    while (!isInterruptionRequested()) {
        if (inputAvailable()) {
            emit keyPressed(getchar());
        }
    }
}
