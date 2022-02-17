CC = clang++

INSTALLDIR = $(PWD)

CPPFLAGS = -D_NO_GUI_ -g -O3 -Weverything -Wno-padded -Wno-c++98-compat -std=c++11 \
           -Wno-exit-time-destructors -Wno-global-constructors \
           -m64 -pipe
CPPLIBS =

OBJFILES = src/control_unit.o \
	   src/cpu.o \
	   src/mem.o \
	   src/mips_sim.o
			
DEPS = 

all: $(OBJFILES)
	$(CC) $(CPPFLAGS) -o mips_sim $(OBJFILES) $(CPPLIBS)
	@echo $(INSTALLDIR)

src/%.o: src/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) -c -o $@ $< 

clean:
	rm -rf src/*.o
