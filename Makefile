CC := g++
MOC := moc
UIC := uic

INSTALLDIR = $(PWD)

ifeq ($(CC), clang++)
  CPPFLAGS = -D_QT -g -O3 -Weverything -Wno-padded -Wno-c++98-compat -std=c++17 \
             -Wno-exit-time-destructors -Wno-global-constructors -Wno-unreachable-code-break \
             -Wno-unused-macros \
             -m64 -pipe
else
  CPPFLAGS = -D_QT -g -O3 -Wpedantic -Wall -Wno-padded -std=c++17 \
             -m64 -pipe
endif

CLICPPFLAGS = -g -O3 -Wall -Wno-padded -std=c++17 -m64 -Wno-weak-vtables
FLEXFLAGS =
BISONFLAGS = -Wall -d

CPPLIBS = 

QTLIBS = -L/usr/lib/x86_64-linux-gnu -lQt5Widgets -lQt5Gui -lQt5Core
QT_INCLUDE_BASE = $(shell qmake -query QT_INSTALL_HEADERS)
QTINCLUDE := -fPIC \
             -I$(QT_INCLUDE_BASE)
QTOBJFILES = src/interface/qt/mips_sim_gui.o \
             src/interface/qt/mips_sim_settings.o \
             src/interface/qt/moc_mips_sim_gui.o \
             src/interface/qt/moc_mips_sim_settings.o
QTFILES = src/interface/qt/ui_mips_sim_gui.h \
          src/interface/qt/moc_mips_sim_gui.cpp \
          src/interface/qt/ui_mips_sim_settings.h \
          src/interface/qt/moc_mips_sim_settings.cpp

OBJFILES = src/assembler/mips_parser.o \
           src/assembler/mips_scanner.o \
           src/cpu/component/alu.o \
           src/cpu/component/control_unit.o \
           src/cpu/component/registers_bank.o \
           src/cpu/cpu.o \
           src/cpu/cpu_multi.o \
           src/cpu/cpu_pipelined.o \
           src/interface/mipscli.o \
           src/mem.o \
           src/utils.o \
           src/mips_sim.o

DEPS =


all: $(OBJFILES) $(QTOBJFILES) $(QTFILES)
	$(CC) $(CPPFLAGS) -o mips_sim $(OBJFILES) $(QTOBJFILES) $(CPPLIBS) $(QTLIBS)
	@echo $(INSTALLDIR)

src/assembler/mips_parser.cpp: src/assembler/mips_assembler.y
	bison $(BISONFLAGS) -o src/assembler/mips_parser.cpp src/assembler/mips_assembler.y

src/assembler/mips_scanner.cpp: src/assembler/mips_assembler.l
	flex $(FLEXFLAGS) -o src/assembler/mips_scanner.cpp src/assembler/mips_assembler.l

src/assembler/%.o: src/assembler/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) -c -o $@ $<

src/interface/qt/%.o: src/interface/qt/%.cpp src/interface/qt/ui_mips_sim_gui.h
	@mkdir -p "$(@D)"
	$(CC) $(QTINCLUDE) $(CLICPPFLAGS) -c -o $@ $<

src/interface/qt/moc_mips_sim_gui.cpp: src/interface/qt/mips_sim_gui.h
	$(MOC) $(INCLUDE) $< -o $@

src/interface/qt/moc_mips_sim_settings.cpp: src/interface/qt/mips_sim_settings.h
	$(MOC) $(INCLUDE) $< -o $@

src/interface/qt/moc_text_browser_stream.cpp: src/interface/qt/text_browser_stream.h
	$(MOC) $(INCLUDE) $< -o $@

src/interface/%.o: src/interface/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CLICPPFLAGS) -c -o $@ $<

src/interface/qt/ui_mips_sim_gui.h: src/interface/qt/mips_sim_gui.ui
	$(UIC) $< -o $@
	
src/interface/qt/ui_mips_sim_settings.h: src/interface/qt/mips_sim_settings.ui
	$(UIC) $< -o $@

src/mips_sim.o: src/mips_sim.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) -D_QT $(CLICPPFLAGS) $(QTINCLUDE) -c -o $@ $<

src/%.o: src/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	find src -name "*.o" | xargs rm -f
	rm src/assembler/mips_scanner.cpp src/assembler/mips_parser.cpp

parser: src/assembler/mips_parser.cpp src/assembler/mips_scanner.cpp
