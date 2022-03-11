CC = clang++

INSTALLDIR = $(PWD)

CPPFLAGS = -g -O3 -Weverything -Wno-padded -Wno-c++98-compat -std=c++14 \
           -Wno-exit-time-destructors -Wno-global-constructors \
					 -Wno-unused-macros \
           -m64 -pipe
CLICPPFLAGS = -g -O3 -Wall -Wno-padded -std=c++14 -m64 -Wno-weak-vtables

CPPLIBS =

OBJFILES = src/assembler/mips_scanner.o \
           src/assembler/mips_parser.o \
           src/cpu/control_unit.o \
	         src/cpu/cpu.o \
					 src/cpu/cpu_multi.o \
					 src/cpu/cpu_pipelined.o \
					 src/interface/mipscli.o \
	         src/mem.o \
		       src/utils.o \
	         src/mips_sim.o

DEPS =


all: $(OBJFILES)
	$(CC) $(CPPFLAGS) -o mips_sim $(OBJFILES) $(CPPLIBS) -lpthread
	@echo $(INSTALLDIR)

src/assembler/mips_scanner.cpp: src/assembler/mips_assembler.l
	flex -o src/assembler/mips_scanner.cpp src/assembler/mips_assembler.l

src/assembler/mips_parser.cpp: src/assembler/mips_assembler.y
	bison -d -o src/assembler/mips_parser.cpp src/assembler/mips_assembler.y

src/assembler/%.o: src/assembler/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) -c -o $@ $<

src/interface/%.o: src/interface/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CLICPPFLAGS) -c -o $@ $<

src/mips_sim.o: src/mips_sim.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CLICPPFLAGS) -c -o $@ $<

src/%.o: src/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	find src -name "*.o" | xargs rm -f
	rm src/assembler/mips_scanner.cpp src/assembler/mips_parser.cpp

parser:
	flex -o src/assembler/mips_scanner.cpp src/assembler/mips_assembler.l
	bison -d -o src/assembler/mips_parser.cpp src/assembler/mips_assembler.y
