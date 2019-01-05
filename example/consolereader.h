#ifndef CONSOLEREADER_H
#define CONSOLEREADER_H

#include <QThread>
#include <termios.h>

class ConsoleReader : public QThread
{
    Q_OBJECT
signals:
    void KeyPressed(char ch);
public:
   ConsoleReader();
   ~ConsoleReader();
   void run();

private:
   struct termios oldSettings;
   struct termios newSettings;

   char getch();
   void resetTermios();
   void initTermios(int echo);
};

#endif  /* CONSOLEREADER_H */
