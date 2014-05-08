UNAME := $(shell uname)

CC = cc
CFLAGS = -std=c99 -Wall
LDFLAGS = -lm

BINARY = awl

SRCDIR = src
OBJDIR = obj
BINDIR = bin
TESTDIR = test

TARGET = $(BINDIR)/$(BINARY)
CODE := $(wildcard $(SRCDIR)/*.c)
ifneq ($(UNAME), Linux)
    CODE := $(filter-out $(SRCDIR)/linenoise.c, $(CODE))
endif
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(CODE:.c=.o)))

TESTTARGET = $(BINDIR)/run-tests
TESTCODE = $(wildcard $(TESTDIR)/*.c)
TESTOBJECTS = $(addprefix $(OBJDIR)/$(TESTDIR)/, $(notdir $(TESTCODE:.c=.o)))


all: $(TARGET)

debug: CFLAGS += -g
debug: $(TARGET)

test: $(TESTTARGET)
	$(TESTTARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TESTDIR):
	mkdir -p $(OBJDIR)/$(TESTDIR)

$(OBJECTS): | $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(TESTOBJECTS): | $(OBJDIR)/$(TESTDIR)

$(OBJDIR)/$(TESTDIR)/%.o: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTTARGET): $(TESTOBJECTS) | $(BINDIR)
	$(CC) $(LDFLAGS) $(TESTOBJECTS) -o $@

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJDIR)/*.o $(OBJDIR)/$(TESTDIR)/*.o $(BINDIR)/*
