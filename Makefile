CC := g++
TARGET := rose_identify

ROSECPP=$(shell rose-config cppflags)
ROSECXX=$(shell rose-config cxxflags) -Wno-misleading-indentation
ROSELD=$(shell rose-config ldflags) -Wl,-rpath=$(rose-config prefix)/lib

SRCDIR := src
OBJDIR := obj
TARGETDIR := target

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(TARGETDIR)
	$(CC) $(ROSECPP) $(ROSECXX) $^ -o $(TARGETDIR)/$@ $(ROSELD)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(ROSECPP) $(ROSECXX) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGETDIR):
	mkdir -p $(TARGETDIR)

clean:
	rm -f $(OBJDIR)/*.o $(TARGETDIR)/*
