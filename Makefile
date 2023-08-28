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

all: include_ressources $(ASMOBJS) $(TARGET)

include_ressources:
	@echo "Running include_ressources..."
	@python include_ressources.py
	@mkdir -p $(OBJDIR)

$(ASMOBJS): $(OBJDIR)/%.o: $(ASMDIR)/%.asm
	@echo "Assembling $<..."
	@as --64 $< -o $@

$(TARGET): $(OBJS) $(ASMOBJS)
	@echo "Linking $(TARGET)..."
	@$(CC) $(CFLAGS) -o $@ $(OBJS) $(ASMOBJS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	@rm -f $(OBJS) $(ASMOBJS) $(TARGET)
