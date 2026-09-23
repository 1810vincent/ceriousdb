
CC = gcc
CFLAGS = -std=c99 -Iinclude -Wall -Wextra -MMD -MP

.PHONY: all build dev release test test-build clean

all: build test


#--------------------
# ceriousapi library
#--------------------

CERIOUSAPI_SRCS = $(wildcard src/ceriousapi/*.c)

TARGET_DEV_LIB = lib/dev/libceriousapi.a
TARGET_RELEASE_LIB = lib/release/libceriousapi.a

CERIOUSAPI_DEV_OBJS = $(patsubst src/ceriousapi/%.c, lib/dev/%.o, $(CERIOUSAPI_SRCS))
CERIOUSAPI_RELEASE_OBJS = $(patsubst src/ceriousapi/%.c, lib/release/%.o, $(CERIOUSAPI_SRCS))

$(TARGET_DEV_LIB): $(CERIOUSAPI_DEV_OBJS)
	ar rcs $@ $^
	@echo "development library ready: $(TARGET_DEV_LIB)"

lib/dev/%.o: src/ceriousapi/%.c
	@mkdir -p lib/dev
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_RELEASE_LIB): $(CERIOUSAPI_RELEASE_OBJS)
	ar rcs $@ $^
	@echo "release library ready: $(TARGET_RELEASE_LIB)"

lib/release/%.o: src/ceriousapi/%.c
	@mkdir -p lib/release
	$(CC) $(CFLAGS) -c $< -o $@


#------------------
# ceriousdb server
#------------------

build: dev release

CERIOUSDB_SRCS = $(wildcard src/ceriousdb/*.c) \
				 src/dotenv/dotenv.c \
				 src/dictionary/str_dictionary.c

TARGET_DEV_SERVER = bin/dev/dev-server
TARGET_RELEASE_SERVER = bin/release/release-server

dev: $(TARGET_DEV_LIB)
dev: CFLAGS += -Werror -O0 -g -fsanitize=address,undefined
dev: $(TARGET_DEV_SERVER)

$(TARGET_DEV_SERVER): $(CERIOUSDB_SRCS) $(TARGET_DEV_LIB)
	@mkdir -p bin/dev
	$(CC) $(CFLAGS) $(CERIOUSDB_SRCS) $(TARGET_DEV_LIB) -o $(TARGET_DEV_SERVER)
	@echo "development build ready: $(TARGET_DEV_SERVER)"


release: $(TARGET_RELEASE_LIB)
release: CFLAGS += -O2 -s -flto
release: $(TARGET_RELEASE_SERVER)

$(TARGET_RELEASE_SERVER): $(CERIOUSDB_SRCS) $(TARGET_RELEASE_LIB)
	@mkdir -p bin/release
	$(CC) $(CFLAGS) $(CERIOUSDB_SRCS) $(TARGET_RELEASE_LIB) -o $(TARGET_RELEASE_SERVER)
	@echo "release build ready: $(TARGET_RELEASE_SERVER)"


#-------
# tests
#-------

test: unit-test

test-build: unit-test-build

TARGET_UNIT_TEST_DOTENV = tests/unit/bin/test_dotenv
TARGET_UNIT_TEST_STR_DICTIONARY = tests/unit/bin/test_str_dictionary

unit-test-build: CFLAGS += -Werror -O0 -g -fsanitize=address,undefined
unit-test-build: $(TARGET_UNIT_TEST_DOTENV) $(TARGET_UNIT_TEST_STR_DICTIONARY)
	@echo "test binaries compiled"

unit-test: unit-test-build
	@echo "running test binaries"
	@$(TARGET_UNIT_TEST_DOTENV)
	@$(TARGET_UNIT_TEST_STR_DICTIONARY)

$(TARGET_UNIT_TEST_DOTENV): tests/unit/test_dotenv.c src/dotenv/dotenv.c
	@mkdir -p tests/unit/bin
	$(CC) $(CFLAGS) tests/unit/test_dotenv.c src/dotenv/dotenv.c -o $(TARGET_UNIT_TEST_DOTENV)

$(TARGET_UNIT_TEST_STR_DICTIONARY): tests/unit/test_str_dictionary.c src/dictionary/str_dictionary.c
	@mkdir -p tests/unit/bin
	$(CC) $(CFLAGS) tests/unit/test_str_dictionary.c src/dictionary/str_dictionary.c -o $(TARGET_UNIT_TEST_STR_DICTIONARY)


#-------------------
# auto-dependencies
#------------------

DEPS = $(wildcard lib/*/*.d) $(wildcard bin/*/*.d)
-include $(DEPS)


#-------
# clean
#-------

clean:
	rm -rf bin/ lib/ tests/unit/bin/
	@echo "all binaries and dependency caches cleaned"