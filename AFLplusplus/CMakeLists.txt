# ===============================================
# Compile afl-fuzz & frida_mode for Android
# Author: Eric Le Guevel
# Date: 2023-03-28
# ===============================================

project(AFLppAndroid)
cmake_minimum_required(VERSION 3.13)

add_compile_options(
    -Wno-pointer-sign
    -Wno-pointer-arith
    -Wno-sign-compare
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-format
    -Wno-user-defined-warnings
    -Wno-macro-redefined
    -Wno-deprecated
)

# ========
# afl-fuzz
# ========

file(GLOB afl-fuzz-c src/afl-fuzz*.c)

add_executable(
    afl-fuzz
    ${afl-fuzz-c}
    "src/afl-common.c"
    "src/afl-forkserver.c"
    "src/afl-sharedmem.c"
    "src/afl-performance.c"
)

target_include_directories(afl-fuzz AFTER PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_definitions(
    afl-fuzz
    PRIVATE DOC_PATH=\"/usr/local/share/doc/afl\"
    PRIVATE BIN_PATH=\"/usr/local/bin\"
    PRIVATE AFL_PATH=\"/usr/local/lib/afl\"
    PRIVATE _FORTIFY_SOURCE=2
)

set_target_properties(afl-fuzz PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(afl-fuzz PROPERTIES COMPILE_FLAGS "-O2")

# ==========
# frida_mode
# ==========

# Retrieves frida-gumjs-devkit

set(FRIDA_LATEST_URL "https://api.github.com/repos/frida/frida/releases/latest")

if (NOT FRIDA_VERSION)
    set(FRIDA_VERSION "16.0.13")
endif()
message("Frida version is set to: ${FRIDA_VERSION}")

set(FRIDA_REPO_URL "https://github.com/frida/frida/releases/download")
set(
    FRIDA_GDK_TARBALL
    "frida-gumjs-devkit-${FRIDA_VERSION}-android-arm64.tar.xz"
)
set(FRIDA_GDK_URL "${FRIDA_REPO_URL}/${FRIDA_VERSION}/${FRIDA_GDK_TARBALL}")
set(FRIDA_DIR "${CMAKE_BINARY_DIR}/frida-${FRIDA_VERSION}")

if(NOT EXISTS ${FRIDA_DIR})
    if(NOT EXISTS ${FRIDA_GDK_TARBALL})
        file(DOWNLOAD ${FRIDA_GDK_URL} ${FRIDA_GDK_TARBALL} SHOW_PROGRESS)
    endif()

    file(ARCHIVE_EXTRACT INPUT ${FRIDA_GDK_TARBALL} DESTINATION ${FRIDA_DIR})
endif()

# Generates api.c from api.js

set(API_JS "${CMAKE_SOURCE_DIR}/frida_mode/src/js/api.js")
set(API_C "api.c")

execute_process(
	COMMAND
	bash -c "echo 'unsigned char api_js[] = {' > ${API_C}; \
	xxd -p -c 12 ${API_JS} | sed -e \"s/\\([0-9a-f]\\{2\\}\\)/0x\\1, /g\" \
	                       | sed -e \"s/^/  /\" >> ${API_C}; \
	echo '};' >> ${API_C}; \
	echo \"unsigned int api_js_len = $(stat --printf='%s' ${API_JS});\" \
    >> ${API_C}"
)

# Generates frida-afl-trace library

file(GLOB_RECURSE FRIDA_MODE_SRCS frida_mode/src/*.c)

add_library(
    afl-frida-trace
    SHARED
    ${FRIDA_MODE_SRCS}
    api.c
    ${CMAKE_SOURCE_DIR}/instrumentation/afl-compiler-rt.o.c
    ${CMAKE_SOURCE_DIR}/src/afl-performance.c
)

target_include_directories(
    afl-frida-trace
    AFTER
    PRIVATE ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/frida_mode/include
    PRIVATE ${FRIDA_DIR}
)

target_link_directories(
    afl-frida-trace
    PRIVATE ${FRIDA_DIR}
)

set_target_properties(afl-frida-trace PROPERTIES PREFIX "")
set_target_properties(afl-frida-trace PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(afl-frida-trace PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(
    afl-frida-trace PROPERTIES LINK_FLAGS
    "-static-libstdc++ -DANDROID -llog -shared -z noexecstack \
    -Wl,--gc-sections -Wl,--exclude-libs,ALL -ldl \
    -Wl,--version-script=${CMAKE_SOURCE_DIR}/frida_mode/frida.map"
)

target_link_libraries(afl-frida-trace frida-gumjs)
