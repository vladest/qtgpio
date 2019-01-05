#include "consolereader.h"
#include <stdio.h>
#include <unistd.h>

/* Initialize new terminal i/o settings */
void ConsoleReader::initTermios(int echo)
{
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

void ConsoleReader::run()
{
    while (!isInterruptionRequested()) {
        char key = getch();
        emit KeyPressed(key);
    }
}
