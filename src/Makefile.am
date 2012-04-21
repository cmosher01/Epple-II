METASOURCES=AUTO



bin_PROGRAMS=epple2

AM_CPPFLAGS=$(all_includes)
epple2_LDFLAGS=$(all_libraries)

epple2_CPPFLAGS = $(AM_CPPFLAGS) -DETCDIR=\"$(sysconfdir)\"

epple2_SOURCES = a2colorsobserved.cpp addressbus.cpp analogtv.cpp apple2.cpp \
	applentsc.cpp card.cpp cassette.cpp clipboardhandler.cpp clockcard.cpp \
	configep2.cpp cpu.cpp diskbytes.cpp diskcontroller.cpp drive.cpp \
	emptyslot.cpp emulator.cpp firmwarecard.cpp gui.cpp hypermode.cpp \
	keyboard.cpp keyboardbuffermode.cpp languagecard.cpp lowpass_1_5_mhz.cpp \
	lowpass_3_58_mhz.cpp main.cpp memory.cpp paddlebuttonstates.cpp \
	paddles.cpp picturegenerator.cpp powerupreset.cpp raminitializer.cpp \
	screenimage.cpp slots.cpp speakerclicker.cpp standardin.cpp \
	standardinproducer.cpp standardout.cpp steppermotor.cpp textcharacters.cpp \
	timable.cpp video.cpp videoaddressing.cpp videomode.cpp \
	videostaticgenerator.cpp SDL_win32_main.c

noinst_HEADERS = a2colorsobserved.h addressbus.h analogtv.h apple2.h applentsc.h \
	card.h cassette.h clipboardhandler.h clockcard.h configep2.h cpu.h diskbytes.h \
	diskcontroller.h drive.h e2const.h emptyslot.h emulator.h firmwarecard.h font3x5.h gui.h \
	hypermode.h keyboardbuffermode.h keyboard.h languagecard.h lowpass_1_5_mhz.h \
	lowpass_3_58_mhz.h memory.h paddlebuttonstates.h paddles.h picturegenerator.h \
	powerupreset.h raminitializer.h screenimage.h slots.h speakerclicker.h \
	standardin.h standardinproducer.h standardout.h steppermotor.h \
	textcharacterimages.h textcharacters.h timable.h util.h \
	videoaddressing.h video.h videomode.h videostaticgenerator.h
