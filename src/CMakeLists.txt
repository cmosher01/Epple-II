set(sources
a2colorsobserved.cpp
addressbus.cpp
analogtv.cpp
apple2.cpp
applentsc.cpp
card.cpp
cassette.cpp
cassettein.cpp
cassetteout.cpp
Circuit.cpp
clipboardhandler.cpp
clockcard.cpp
Common.cpp
configep2.cpp
Cpu6502.cpp
Cpu6502Helper.cpp
cpu.cpp
disk2readwritehead.cpp
disk2steppermotorcan.cpp
disk2steppermotor.cpp
disk2steppermotorrotor.cpp
diskcontroller.cpp
drive.cpp
drivemotor.cpp
emptyslot.cpp
Emu6502.cpp
emulator.cpp
filterchroma.cpp
filterluma.cpp
firmwarecard.cpp
gui.cpp
hypermode.cpp
keyboardbuffermode.cpp
keyboard.cpp
languagecard.cpp
lss.cpp
magneticfield.cpp
main.cpp
memorychip.cpp
memory.cpp
memoryrandomaccess.cpp
memoryrow.cpp
memorystrapping.cpp
movable.cpp
paddlebuttonstates.cpp
paddles.cpp
picturegenerator.cpp
powerupreset.cpp
screenimage.cpp
SegmentCache.cpp
slots.cpp
speakerclicker.cpp
standardin.cpp
standardinproducer.cpp
standardout.cpp
StateCalculator.cpp
textcharacters.cpp
timable.cpp
tinyfiledialogs.cpp
Trace.cpp
TransCache.cpp
TransNetwork.cpp
videoaddressing.cpp
video.cpp
videomode.cpp
videostaticgenerator.cpp
wozfile.cpp
)

add_executable(epple2 ${sources})

find_package(SDL2 CONFIG)
message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
target_include_directories(epple2 PRIVATE ${SDL2_INCLUDE_DIRS})
message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
target_link_libraries(epple2 ${SDL2_LIBRARIES})

target_compile_features(epple2 PRIVATE cxx_std_17)
target_compile_definitions(epple2 PRIVATE ETCDIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}")
