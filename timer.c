#include <stdlib.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <string.h>
#include <time.h>

enum time_type {
  SECONDS,
  MINUTES,
  HOURS
};

void print_usage(FILE* target) {
  if (fputs("Usage: timer -time_unit time\n"
            "  time_unit: time unit, h for hours, m for minutes, s for seconds\n"
            "  time: time in the given unit\n"
            "Example: timer -m 10\n",
            target) < 0)
    abort();
}

noreturn void input_error(char* message) {
  if (fputs(message, stderr) < 0)
    abort();
  if (fputc('\n', stderr) < 0)
    abort();
  print_usage(stderr);
  exit(EXIT_FAILURE);
}

enum time_type read_time_unit(char* str) {
  if (strlen(str) != 2)
    input_error("Time unit argument does not have length 2.");
  if (str[0] != '-')
    input_error("Time unit argument has to start with `-`.");
  switch (str[1]) {
  case 's':
    return SECONDS;
  case 'm':
    return MINUTES;
  case 'h':
    return HOURS;
  default:
    input_error("Unrecognized time unit.");
  }
}

void get_sleep_time(struct timespec* sleep_time, enum time_type time_type, char* str) {
  long parsed_time = atol(str);
  if (parsed_time <= 0)
    input_error("Invalid time.");
  sleep_time->tv_nsec = 0;
  switch (time_type) {
  case HOURS:
    parsed_time *= 60;
  case MINUTES:
    parsed_time *= 60;
  case SECONDS:
    sleep_time->tv_sec = parsed_time;
    return;
  }
}

noreturn void timer_alert() {
  if (puts("Timer ran out.") < 0)
    abort();
  /* 100 milliseconds */
  struct timespec beep_interval = { .tv_sec = 0, .tv_nsec = 100000000 };
  while (1) {
    if (putchar('\a') < 0)
      abort();
    if (fflush(stdout) < 0)
      abort();
    if (nanosleep(&beep_interval, NULL) < 0) {
      perror("`nanosleep` failed");
      abort();
    }
  }
}

noreturn void sleep_error() {
  perror("\aSleeping for timer duration failed");
  abort();
}

int main(int argc, char** argv) {
  if (argc == 1) {
    print_usage(stdout);
    exit(EXIT_SUCCESS);
  }
  if (argc != 3)
    input_error("Wrong argument count.");
  enum time_type time_type = read_time_unit(argv[1]);
  struct timespec sleep_time;
  get_sleep_time(&sleep_time, time_type, argv[2]);
  if (nanosleep(&sleep_time, NULL) < 0)
    sleep_error();
  timer_alert();
}
