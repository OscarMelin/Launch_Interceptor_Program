
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -ffloat-store
LDFLAGS =
SOURCE = decide.c
TARGET = decide.o
COVERAGE_OUT = coverage.info
CODE_COVERAGE_TOOL = lcov
HTML_DIR = html_gen

all: CFLAGS += -O2 -c
all: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE)

test: CFLAGS += --coverage -D__need_clockid_t 
test: LDFLAGS += -lm 
test: SOURCE += test.c
test: TARGET = run_test
test: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(LDFLAGS) -o $(TARGET)
	$(CODE_COVERAGE_TOOL) -z -d .
	./$(TARGET)
	$(CODE_COVERAGE_TOOL) -c -d . -o $(COVERAGE_OUT)
	mkdir $(HTML_DIR)
	genhtml $(COVERAGE_OUT) -o $(HTML_DIR)

clean:
	rm -rf *.o *.gcno *.gcda run_test $(COVERAGE_OUT) $(HTML_DIR)

