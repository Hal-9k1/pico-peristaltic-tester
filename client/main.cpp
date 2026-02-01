#include <fcntl.h>
#include <iostream>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#define STRINGIFY(x) #x
#define VTCTRL "\033["
#define CURSOR_UP(n) VTCTRL STRINGIFY(n) "A"
#define CURSOR_DOWN(n) VTCTRL STRINGIFY(n) "B"
#define CLEAR_RIGHT VTCTRL "K"
#define CARRIAGE_RETURN "\r"
#define INVALID_FILE -1
#define PICO_SERIAL_PATH "/dev/ttyS1"
#define SERIAL_BAUD B115200
#define STEPS_LINE 4
#define SPEED_LINE 3
#define DIRECTION_LINE 2

static termios origTermConf;
int picoSerial = INVALID_FILE;
char DIRECTION_TOGGLE = 255;

void restoreTerminalState()
{
  tcsetattr(fileno(stdin), 0, &origTermConf);
}

void updateSpeed(unsigned char speed)
{
  std::cout << CURSOR_UP(SPEED_LINE) << CLEAR_RIGHT << "Current speed: " << (int)speed
    << CARRIAGE_RETURN << CURSOR_DOWN(SPEED_LINE);
}

void updateDirection(bool forward)
{
  std::cout << CURSOR_UP(DIRECTION_LINE) << CLEAR_RIGHT
    << (const char *)(forward ? "Forward" : "Reverse") << CARRIAGE_RETURN
    << CURSOR_DOWN(DIRECTION_LINE);
}

void updateSteps(unsigned int *pSteps, int modifier)
{
  if ((long)-modifier > (long)*pSteps)
  {
    *pSteps = 0;
  }
  else if (*pSteps > INT_MAX - modifier)
  {
    *pSteps = INT_MAX;
  }
  else
  {
    *pSteps += modifier;
  }
  std::cout << CURSOR_UP(STEPS_LINE) << CLEAR_RIGHT
    << "Steps: " << *pSteps << CARRIAGE_RETURN << CURSOR_DOWN(STEPS_LINE);
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

int getRepeatIncrement(int repeats)
{
  return repeats >= 36 ? INT_MAX : (int)exp2(fmax(repeats - 4, 0));
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
    << "u: increase steps     i: decrease steps" << std::endl
    << "U: increase steps+    I: decrease steps+" << std::endl
    << "k: increase speed     j: decrease speed" << std::endl
    << "n: toggle direcion    <space>: run pump" << std::endl
    << "q: quit" << "\n\n\n\n\n\n" << std::endl;

  char c;
  char lastChr = 0;
  bool isForward = true;
  unsigned char speed = 16;
  int repeats = 0;
  unsigned int steps = 4000;
  updateSteps(&steps, 0);
  updateSpeed(speed);
  updateDirection(isForward);
  while (std::cin.get(c))
  {
    if (c == 'k' && speed != 254)
    {
      ++speed;
      updateSpeed(speed);
    }
    else if (c == 'j' && speed != 0)
    {
      --speed;
      updateSpeed(speed);
    }
    else if (c == 'u')
    {
      updateSteps(&steps, -1);
    }
    else if (c == 'i')
    {
      updateSteps(&steps, 1);
    }
    else if (c == 'U')
    {
      if (lastChr == 'U')
      {
        ++repeats;
      }
      else
      {
        repeats = 0;
      }
      updateSteps(&steps, -getRepeatIncrement(repeats));
    }
    else if (c == 'I')
    {
      if (lastChr == 'I')
      {
        ++repeats;
      }
      else
      {
        repeats = 0;
      }
      updateSteps(&steps, getRepeatIncrement(repeats));
    }
    else if (c == 'n')
    {
      isForward = !isForward;
      updateDirection(isForward);
    }
    else if (c == ' ')
    {
      if (picoSerial == INVALID_FILE)
      {
        std::cout << "Serial line is not open." << std::flush;
        usleep(750000);
        std::cout << CARRIAGE_RETURN << CLEAR_RIGHT << std::flush;
      }
      else
      {
        unsigned char buf[] = {
          speed,
          isForward,
          (unsigned char)(steps >> 24),
          (unsigned char)((steps >> 16) & 0xff),
          (unsigned char)((steps >> 8) & 0xff),
          (unsigned char)steps
        };
        write(picoSerial, buf, 6);
        std::cout << "Command sent." << std::flush;
        usleep(750000);
        std::cout << CARRIAGE_RETURN << CLEAR_RIGHT << std::flush;
      }
    }
    else if (c == 'q')
    {
      break;
    }
    lastChr = c;
  }

  cleanup();
}
