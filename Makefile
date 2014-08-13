UNAME := $(shell uname)

# Compilation options
#
CC ?= cc
CFLAGS ?= -std=c11 -Wall -pedantic
LDFLAGS ?= -lm

# Binary and directory names
#
BINARY = awl
TESTBINARY = run-tests

SRCDIR = src
TESTDIR = test
BINDIR = bin
WEBDIR = web
OBJDIR = obj
LIBDIR = lib
MAINOBJDIR = $(OBJDIR)/$(BINARY)
TESTOBJDIR = $(OBJDIR)/$(TESTDIR)

TARGET = $(BINDIR)/$(BINARY)
BITCODE = $(TARGET).bc
WEBTARGET = $(WEBDIR)/$(BINARY).js
WEBMAP = $(WEBTARGET).map
WEBFUNCS = "['_setup_awl','_teardown_awl','_get_awl_version','_register_print_fn','_awlenv_new_top_level','_eval_repl_str']"
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
TESTOBJECTS = $(addprefix $(TESTOBJDIR)/, $(notdir $(TESTCODE:.c=.o))) $(filter-out $(MAINOBJDIR)/main.o, $(OBJECTS))
TESTDEPS = $(TESTCODE:.c=.d)

CLEAN = rm -f $(TARGET) $(BITCODE) $(WEBTARGET) $(WEBMAP) $(OBJDIR)/*.o $(MAINOBJDIR)/*.o $(TESTOBJDIR)/*.o $(BINDIR)/*

.PHONY: debug release clean web

all: debug

debug: CFLAGS += -g
debug: $(TARGET)

release: CFLAGS += -O2
release: $(TARGET)

web: CFLAGS += -g
web: webmake

webrelease: CFLAGS += -O2
webrelease: webmake
	@type uglifyjs >/dev/null 2>&1 || { echo "UglifyJS not found, not using"; exit 1; }
	uglifyjs $(WEBTARGET) --in-source-map $(WEBMAP) -o $(WEBTARGET)

webmake: clean $(WEBTARGET)
	$(CLEAN)
	mv $(WEBTARGET).tmp $(WEBTARGET)
	mv $(WEBMAP).tmp $(WEBMAP)

$(WEBTARGET): CC := emcc
$(WEBTARGET): LDFLAGS := -g --preload-file $(LIBDIR) -s EXPORTED_FUNCTIONS=$(WEBFUNCS) -s RESERVED_FUNCTION_POINTERS=1
$(WEBTARGET): $(TARGET)
	mv $(TARGET) $(BITCODE)
	$(CC) $(LDFLAGS) $(BITCODE) -o $(WEBTARGET)
	mv $(WEBTARGET) $(WEBTARGET).tmp
	mv $(WEBMAP) $(WEBMAP).tmp

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
	$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $@

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
	$(CC) $(TESTOBJECTS) $(LDFLAGS) -o $@

clean:
	$(CLEAN)
