CC = clang++

INSTALLDIR = $(PWD)

CPPFLAGS = -D_NO_GUI_ -g -O3 -Weverything -Wno-padded -Wno-c++98-compat -std=c++11 \
           -Wno-exit-time-destructors -Wno-global-constructors \
           -m64 -pipe
CPPLIBS =

OBJFILES = src/assembler/assembler.o \
           src/cpu/control_unit.o \
	         src/cpu/cpu.o \
					 src/cpu/cpu_multi.o \
					 src/cpu/cpu_pipelined.o \
	         src/mem.o \
		       src/utils.o \
	         src/mips_sim.o

DEPS =

all: $(OBJFILES)
	$(CC) $(CPPFLAGS) -o mips_sim $(OBJFILES) $(CPPLIBS)
	@echo $(INSTALLDIR)

src/%.o: src/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	find src -name "*.o" | xargs rm -f
