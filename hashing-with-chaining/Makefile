# Compiler
CC := g++
CFLAGS := -g -Wall -std=c++11

# Directories 
SRCDIR := src
BUILDDIR := build
TARGET := bin/a.out

# Pre-compiling
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
INC := -I include

$(TARGET) : $(OBJECTS)
	@mkdir -p "bin"
	@echo " Linking..."; $(CC) $^ -o $(TARGET)
	@echo " Done!"

$(BUILDDIR)/%.o : $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " Generating: $@"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; $(RM) -r $(BUILDDIR) $(TARGET)
	@echo " Done!"

info:
	@echo " Info..."
	@echo " SOURCES = $(SOURCES)"
	@echo " OBJECTS = $(OBJECTS)"