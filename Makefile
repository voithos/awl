UNAME := $(shell uname)

# Compilation options
#
CC := cc
CFLAGS = -std=c11 -Wall -pedantic
LDFLAGS = -lm

# Binary and directory names
#
BINARY = awl
TESTBINARY = run-tests

SRCDIR = src
TESTDIR = test
BINDIR = bin
OBJDIR = obj
MAINOBJDIR = $(OBJDIR)/$(BINARY)
TESTOBJDIR = $(OBJDIR)/$(TESTDIR)

TARGET = $(BINDIR)/$(BINARY)
BITCODE = $(TARGET).bc
WEBTARGET = $(BINARY).js
WEBMAP = $(WEBTARGET).map
CODE := $(wildcard $(SRCDIR)/*.c)
HEADERS := $(wildcard $(SRCDIR)/*.h)
ifneq ($(UNAME), Linux)
    CODE := $(filter-out $(SRCDIR)/linenoise.c, $(CODE))
    HEADERS := $(filter-out $(SRCDIR)/linenoise.h, $(HEADERS))
endif
OBJECTS = $(addprefix $(MAINOBJDIR)/, $(notdir $(CODE:.c=.o)))
DEPS = $(CODE:.c=.d)

TESTTARGET = $(BINDIR)/$(TESTBINARY)
TESTCODE = $(wildcard $(TESTDIR)/*.c)
TESTHEADERS = $(wildcard $(TESTDIR)/*.h)
TESTOBJECTS = $(addprefix $(TESTOBJDIR)/, $(notdir $(TESTCODE:.c=.o))) $(filter-out $(MAINOBJDIR)/awl.o, $(OBJECTS))
TESTDEPS = $(TESTCODE:.c=.d)

CLEAN = rm -f $(TARGET) $(BITCODE) $(WEBTARGET) $(WEBMAP) $(OBJDIR)/*.o $(MAINOBJDIR)/*.o $(TESTOBJDIR)/*.o $(BINDIR)/*

.PHONY: debug release clean web

all: debug

debug: CFLAGS += -g
debug: $(TARGET)

release: CFLAGS += -O3
release: $(TARGET)

web: CC := emcc
web: clean $(WEBTARGET)
	$(CLEAN)
	mv $(WEBTARGET).tmp $(WEBTARGET)

$(WEBTARGET): $(TARGET)
	mv $(TARGET) $(BITCODE)
	$(CC) $(BITCODE) -o $(WEBTARGET)
	mv $(WEBTARGET) $(WEBTARGET).tmp

test: $(TESTTARGET)
	$(TESTTARGET)

# Directory creation
$(BINDIR) $(OBJDIR) $(MAINOBJDIR) $(TESTOBJDIR):
	mkdir -p $@

# Dependency management
$(SRCDIR)/%.d: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -MM -MG -MT"$@" -MT"$(<:$(SRCDIR)%.c=$(MAINOBJDIR)%.o)" -MF"$@" "$<"

-include $(DEPS)

$(OBJECTS): | $(MAINOBJDIR)

# Compilation
$(MAINOBJDIR)/%.o:
	$(CC) $(CFLAGS) -c $(@:$(MAINOBJDIR)%.o=$(SRCDIR)%.c) -o $@

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Tests dependency management
$(TESTDIR)/%.d: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) -MM -MG -MT"$@" -MT"$(<:$(TESTDIR)%.c=$(TESTOBJDIR)%.o)" -MF"$@" "$<"

-include $(TESTDEPS)

$(TESTOBJECTS): | $(TESTOBJDIR)

# Tests compilation
$(TESTOBJDIR)/%.o:
	$(CC) $(CFLAGS) -c $(@:$(TESTOBJDIR)%.o=$(TESTDIR)%.c) -o $@

$(TESTTARGET): CFLAGS += -g
$(TESTTARGET): $(TESTOBJECTS) | $(BINDIR)
	$(CC) $(LDFLAGS) $(TESTOBJECTS) -o $@

clean:
	$(CLEAN)
