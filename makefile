TARGET   = hexane

CC       = gcc
LINKER   = gcc

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
TESTDIR  = test

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

TEST_F_FILES := $(wildcard $(TESTDIR)/Framework/*.c)
TEST_F_HEADERS := $(wildcard $(TESTDIR)/Framework/*.h)
TEST_T_FILES := $(wildcard $(TESTDIR)/UnitTests/*.c)
TEST_M_FILES := $(filter-out $(SRCDIR)/Main.c, $(SOURCES))

CFLAGS   = -std=c99 -Wall -I. -I$(SRCDIR)
LFLAGS   = -Wall -I. -lm

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

test: $(TESTDIR)/testgo
	$(TESTDIR)/testgo
	
testclean:
	@rm $(TESTDIR)/testgo

$(TESTDIR)/testgo: $(TEST_F_FILES) $(TEST_F_HEADERS) $(TEST_T_FILES) $(TEST_M_FILES)
	@$(CC) $(CFLAGS) $(TEST_F_FILES) $(TEST_T_FILES) $(TEST_M_FILES) -I$(TESTDIR)/Framework -o $@
	@echo "Unit Test build Done."

.PHONY: clean
clean:
	@rm -f $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
