ifeq ($(OS),Windows_NT)
	CC = g++
	AS = as
else ifeq ($(shell uname -s),Linux)
	CC = x86_64-w64-mingw32-g++
	AS = x86_64-w64-mingw32-as

endif

CFLAGS = -Iinclude -Llib -O3
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -static-libstdc++  



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
	@$(AS) --64 $< -o $@

$(TARGET): $(OBJS) $(ASMOBJS)
	@echo "Linking $(TARGET)..."
	@$(CC) $(CFLAGS) -o $@ $(OBJS) $(ASMOBJS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	@rm -f $(OBJS) $(ASMOBJS) $(TARGET)
