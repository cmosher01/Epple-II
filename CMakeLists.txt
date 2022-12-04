cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL "Echo make commands.")
set(PACKAGE_SUITE TRUE CACHE BOOL "Build entire suite of packages; otherwise just ZIP file.")

#set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

set(APP_NAME epple2)

project(${APP_NAME}
    VERSION 2.0.0
    DESCRIPTION "Apple II emulator"
    HOMEPAGE_URL https://github.com/cmosher01/Epple-II
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_FOURCC epl2)
set(PROJECT_VENDOR "nu.mine.mosher")
set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
set(CPACK_PACKAGE_CONTACT "cmosher01@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")



file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory; make a build subdirectory.")
endif()



if(WIN32)
    set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}")
    unset(CPACK_PACKAGING_INSTALL_PREFIX)
elseif(APPLE)
    unset(CMAKE_INSTALL_PREFIX)
    unset(CPACK_PACKAGING_INSTALL_PREFIX)
else()
    set(CMAKE_INSTALL_PREFIX "/opt/${CPACK_PACKAGE_VENDOR}/${PROJECT_NAME}")
    set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

include(GNUInstallDirs)

include_directories(BEFORE "include")



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
E2wxApp.cpp
E2wxFrame.cpp
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
list(TRANSFORM sources PREPEND "src/")

set(resources
#epple2.xrc
#epple2.png
conf/epple2.conf
)

add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE ${sources} ${resources})

find_package(SDL2 REQUIRED)
target_include_directories(${APP_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${APP_NAME} PRIVATE ${SDL2_LIBRARIES})

find_package(Boost REQUIRED COMPONENTS log filesystem)
# These settings must match those of the installed boost libraries.
# Use objdump to check the "namespace", such as "v2_mt_nt6"
# https://www.boost.org/doc/libs/master/libs/log/doc/html/log/rationale/namespace_mangling.html
target_compile_definitions(${APP_NAME} PRIVATE
    BOOST_ALL_DYN_LINK
    BOOST_ATOMIC_DYN_LINK
    BOOST_CHRONO_DYN_LINK
    BOOST_FILESYSTEM_DYN_LINK
    BOOST_LIB_DIAGNOSTIC
    BOOST_LOG_DYN_LINK
    BOOST_REGEX_DYN_LINK
    BOOST_THREAD_DYN_LINK
    BOOST_ALL_NO_LIB
    BOOST_LIB_DIAGNOSTIC
    BOOST_USE_WINAPI_VERSION=0x0600)
target_link_libraries(${APP_NAME} PRIVATE ${Boost_LIBRARIES})

find_package(wxWidgets REQUIRED COMPONENTS base core xrc qa)
include(${wxWidgets_USE_FILE})
target_link_libraries(${APP_NAME} PRIVATE ${wxWidgets_LIBRARIES})



target_compile_definitions(${APP_NAME} PRIVATE ETCDIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}")



set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${resources}")


