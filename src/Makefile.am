METASOURCES=AUTO



bin_PROGRAMS=epple2

AM_CXXFLAGS=-std=c++11 -Wall -O3
AM_CPPFLAGS=$(all_includes)
epple2_LDFLAGS=$(all_libraries)

epple2_CPPFLAGS = $(AM_CPPFLAGS) -DETCDIR=\"$(sysconfdir)\"

epple2_SOURCES = a2colorsobserved.cpp addressbus.cpp analogtv.cpp apple2.cpp \
applentsc.cpp card.cpp cassette.cpp cassettein.cpp cassetteout.cpp \
clipboardhandler.cpp clockcard.cpp \
configep2.cpp cpu.cpp diskcontroller.cpp drive.cpp drivemotor.cpp \
disk2readwritehead.cpp disk2steppermotor.cpp disk2steppermotorcan.cpp disk2steppermotorrotor.cpp \
magneticfield.cpp movable.cpp \
emptyslot.cpp emulator.cpp firmwarecard.cpp gui.cpp hypermode.cpp \
keyboard.cpp keyboardbuffermode.cpp languagecard.cpp filterchroma.cpp \
filterluma.cpp lss.cpp main.cpp memory.cpp \
memorychip.cpp memoryrow.cpp memorystrapping.cpp memoryrandomaccess.cpp \
paddlebuttonstates.cpp \
paddles.cpp picturegenerator.cpp powerupreset.cpp \
screenimage.cpp slots.cpp speakerclicker.cpp standardin.cpp \
standardinproducer.cpp standardout.cpp textcharacters.cpp \
timable.cpp video.cpp videoaddressing.cpp videomode.cpp \
videostaticgenerator.cpp wozfile.cpp \
Circuit.cpp  Common.cpp  Cpu6502.cpp  Cpu6502Helper.cpp  Emu6502.cpp  SegmentCache.cpp  \
StateCalculator.cpp  Trace.cpp  TransCache.cpp  TransNetwork.cpp \
tinyfiledialogs.cpp

noinst_HEADERS = a2colorsobserved.h addressbus.h analogtv.h apple2.h applentsc.h \
card.h cassette.h cassettein.h cassetteout.h \
clipboardhandler.h clockcard.h configep2.h cpu.h \
disk2readwritehead.h disk2steppermotor.h disk2steppermotorcan.h disk2steppermotorrotor.h \
magneticfield.h movable.h \
diskcontroller.h drive.h drivemotor.h e2const.h emptyslot.h emulator.h firmwarecard.h font3x5.h gui.h \
hypermode.h keyboardbuffermode.h keyboard.h languagecard.h filterchroma.h \
filterluma.h lss.h memory.h \
memorychip.h memoryrow.h memorystrapping.h memoryrandomaccess.h \
paddlebuttonstates.h paddles.h picturegenerator.h \
powerupreset.h screenimage.h slots.h speakerclicker.h \
standardin.h standardinproducer.h standardout.h \
textcharacterimages.h textcharacters.h timable.h util.h \
videoaddressing.h video.h videomode.h videostaticgenerator.h wozfile.h \
Circuit.h  Common.h  Cpu6502.h  Cpu6502Helper.h  Emu6502.h  SegmentCache.h  SegmentTypes.h  \
StateCalculator.h  Trace.h  TransCache.h  TransNetwork.h  addressbus.h  ptr_less.h  trans.h \
tinyfiledialogs.h
