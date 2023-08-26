CC = g++
CFLAGS = -Iinclude -Llib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

SRCDIR = src
OBJDIR = obj
ASMDIR = asm
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
ASMSRCS = $(wildcard $(ASMDIR)/*.asm)
ASMOBJS = $(patsubst $(ASMDIR)/%.asm,$(OBJDIR)/%.o,$(ASMSRCS))

TARGET = main

all: include_ressources $(TARGET)

include_ressources:
	python include_ressources.py
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJS) $(ASMOBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(ASMOBJS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(ASMDIR)/%.asm
	mkdir -p $(OBJDIR)
	as --64 $< -o $@

clean:
	rm -f $(OBJS) $(ASMOBJS) $(TARGET)
