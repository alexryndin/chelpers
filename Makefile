CFLAGS=-g -O2 -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -DNDEBUG -Isrc $(OPTFLAGS)
LDLIBS=-pedantic  $(OPTLIBS)
PREFIX?=/usr/local

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

LIB_SRC=$(wildcard src/*.c)
LIB=$(patsubst %.c,%.o,$(LIB_SRC))
LIB:=$(filter-out src/bbstrlib%, $(LIB))

all: $(LIB) tests

dev: CFLAGS := $(filter-out -O2,$(CFLAGS))
dev: CFLAGS := $(filter-out -DNDEBUG,$(CFLAGS))
dev: CFLAGS := $(filter-out -pedantic,$(CFLAGS))
dev: all

$(BIN): $(LIB)

$(TESTS): $(LIB)

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

# The Unit Tests
.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: CFLAGS := $(filter-out -pedantic,$(CFLAGS))
tests: CFLAGS := $(filter-out -DNDEBUG,$(CFLAGS))
tests: $(TESTS)
	sh ./tests/runtests.sh


build:
	@mkdir -p build

# The Cleaner
clean:
	@echo $(EXTERNAL_SRC_NO_TESTS)
	rm -rf build $(OBJECTS) $(TESTS) $(BIN) $(LIB) $(EXTERNAL)
	rm -f tests/tests.log
	find . -name "*.gc*" -delete
