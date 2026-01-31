#include <cstdio>
#include <termios.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>

#define STRINGIFY(x) #x
#define VTCTRL "\033["
#define CURSOR_UP(n) VTCTRL STRINGIFY(n) "A"
#define CURSOR_DOWN(n) VTCTRL STRINGIFY(n) "B"
#define CLEAR_RIGHT VTCTRL "K"
#define CARRIAGE_RETURN "\r"
#define INVALID_FILE -1
#define PICO_SERIAL_PATH "/dev/ttyS1"
#define SERIAL_BAUD B9600
#define SPEED_LINE 3
#define STATUS_LINE 2

static termios origTermConf;
int picoSerial = INVALID_FILE;
char TOGGLE = 255;

void restoreTerminalState()
{
  tcsetattr(fileno(stdin), 0, &origTermConf);
}

void updateSpeed(unsigned char *pSpeed)
{
  std::cout << CURSOR_UP(SPEED_LINE) << CLEAR_RIGHT << "Current speed: " << (int)*pSpeed
    << CARRIAGE_RETURN << CURSOR_DOWN(SPEED_LINE);
  if (picoSerial != INVALID_FILE)
  {
    write(picoSerial, pSpeed, 1);
  }
}

void updateStatus(bool running)
{
  std::cout << CURSOR_UP(STATUS_LINE) << CLEAR_RIGHT
    << (const char *)(running ? "Running" : "Stopped") << CARRIAGE_RETURN
    << CURSOR_DOWN(STATUS_LINE);
  if (picoSerial != INVALID_FILE)
  {
    write(picoSerial, &TOGGLE, 1);
  }
}

void cleanup()
{
  restoreTerminalState();
  if (picoSerial != INVALID_FILE)
  {
    close(picoSerial);
  }
  std::exit(0);
}

void interrupt(int)
{
  cleanup();
}

int main()
{
  std::cout << std::endl;

  // gracefully exit on ctrl-c
  signal(SIGINT, interrupt);

  // disable input echoing and line editing
  tcgetattr(fileno(stdin), &origTermConf);
  termios termConf(origTermConf);
  termConf.c_lflag &= ~(ECHO | ICANON);
  termConf.c_cc[VMIN] = 1;
  termConf.c_cc[VTIME] = 0;
  tcsetattr(fileno(stdin), 0, &termConf);

  // open serial comms
  picoSerial = open(PICO_SERIAL_PATH, O_WRONLY | O_NOCTTY | O_SYNC);
  if (picoSerial != INVALID_FILE)
  {
    // serial setup doohickery
    termios serialConf;
    tcgetattr(picoSerial, &serialConf);
    cfsetospeed(&serialConf, SERIAL_BAUD);
    cfsetispeed(&serialConf, SERIAL_BAUD);
    serialConf.c_cflag &= (serialConf.c_cflag & ~CSIZE) | CS8;
    serialConf.c_lflag = 0;
    serialConf.c_oflag = 0;
    tcsetattr(picoSerial, 0, &serialConf);
  }
  else
  {
    std::cout << "Failed to open serial!" << std::endl;
  }

  std::cout
    << "j: increase speed    k: decrease speed" << std::endl
    << "u: start/stop        q: quit"
    << std::endl << std::endl << std::endl << std::endl << std::endl;

  char c;
  bool isRunning = true;
  unsigned char speed = 16;
  updateSpeed(&speed);
  updateStatus(isRunning);
  while (std::cin.get(c))
  {
    if (c == 'k' && speed != 254)
    {
      ++speed;
      updateSpeed(&speed);
    }
    else if (c == 'j' && speed != 0)
    {
      --speed;
      updateSpeed(&speed);
    }
    else if (c == 'u')
    {
      isRunning = !isRunning;
      updateStatus(isRunning);
    }
    else if (c == 'q')
    {
      break;
    }
  }

  cleanup();
}
