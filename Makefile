ifeq ($(OS),Windows_NT)
	CC = g++
	AS = as
else ifeq ($(shell uname -s),Linux)
	CC = x86_64-w64-mingw32-g++
	AS = x86_64-w64-mingw32-as

endif

ifeq ($(MAKECMDGOALS),debug)
  CFLAGS += -DDEBUG
endif

CFLAGS = -Iinclude -Iinclude/glib-2.0 -Iinclude/glib-2.0 -Llib -Llib/SDL2 -O3
LIBS = -lmingw32 -lavutil -lswscale -lavformat -lavcodec -lavfilter -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -static-libstdc++  

export PKG_CONFIG_PATH := $(shell pwd)/pkgconfig/
SDL= `pkg-config --cflags --libs sdl2 SDL2_image`

SRCDIR = src
OBJDIR = obj
ASMDIR = asm
BUILDDIR = build
DLLDIR = dll
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
ASMSRCS = $(wildcard $(ASMDIR)/*.asm)
ASMOBJS = $(patsubst $(ASMDIR)/%.asm,$(OBJDIR)/%.o,$(ASMSRCS))

#if build directory does not exist, create it
ifeq ($(wildcard $(BUILDDIR)),)
$(shell mkdir -p $(BUILDDIR))
endif
TARGET = $(BUILDDIR)/main.exe

# copy dll directory to the build directory
#$(shell cp -r $(DLLDIR)/*.dll $(BUILDDIR))

all: include_ressources .WAIT $(ASMOBJS) $(TARGET)

debug: 	CFLAGS += -DDEBUG -g
debug: 	all

include_ressources:
	@echo "Running include_ressources..."
	@python include_ressources.py
	@echo "Running convert_fonts..."
	@python convert_fonts.py
	@echo "Running include_fonts..."
	@python include_fonts.py
	@echo "Running include_tilesets..."
	@python include_tilesets.py
	@echo "Running include_maps..."
	@python include_maps.py
	@mkdir -p $(OBJDIR)

.NOTPARALLEL: include_ressources

$(ASMOBJS): $(OBJDIR)/%.o: $(ASMDIR)/%.asm
	@echo "Assembling $<..."
	@$(AS) --64 $< -o $@

$(TARGET): $(OBJS) $(ASMOBJS)
	@echo "Linking $(TARGET)..."
	@$(CC) $(CFLAGS) -o $@ $(OBJS) $(ASMOBJS) $(SDL) $(LIBS) 
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	@rm -f $(OBJS) $(ASMOBJS) $(TARGET) $(BUILDDIR)/*
