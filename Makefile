CC := g++
TARGET := rose_injector
DEBUG_TARGET := $(TARGET)_debug

ROSECPP=$(shell rose-config cppflags)
ROSECXX=$(shell rose-config cxxflags) -Wno-misleading-indentation
ROSELD=$(shell rose-config ldflags) -Wl,-rpath=$(rose-config prefix)/lib

SRCDIR := src
OBJDIR := obj
TARGETDIR := target

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
DEBUG_OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/debug/%.o,$(SOURCES))

.PHONY: all clean

all: $(TARGET)

debug: $(DEBUG_TARGET)

$(TARGET): $(OBJECTS) | $(TARGETDIR)
	$(CC) $(ROSECPP) $(ROSECXX) $^ -o $(TARGETDIR)/$@ $(ROSELD)

$(DEBUG_TARGET): $(DEBUG_OBJECTS) | $(TARGETDIR)
	$(CC) $(ROSECPP) $(ROSECXX) -g $^ -o $(TARGETDIR)/$@ $(ROSELD)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(ROSECPP) $(ROSECXX) -c $< -o $@

$(OBJDIR)/debug/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)/debug
	$(CC) $(ROSECPP) $(ROSECXX) -g -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/debug:
	mkdir -p $(OBJDIR)/debug

$(TARGETDIR):
	mkdir -p $(TARGETDIR)

clean:
	rm -f $(OBJDIR)/*.o $(TARGETDIR)/*
