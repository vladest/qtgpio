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
   virtual ~ConsoleReader();

protected:
   void run() override;

private:
   struct termios oldSettings;
   struct termios newSettings;

   char getch();
   void resetTermios();
   void initTermios(int echo);
};

#endif  /* CONSOLEREADER_H */
