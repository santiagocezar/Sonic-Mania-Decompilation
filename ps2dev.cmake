if(DEFINED ENV{PS2DEV})
    SET(PS2DEV $ENV{PS2DEV})
else()
    message(FATAL_ERROR "The environment variable PS2DEV needs to be defined.")
endif()

if(DEFINED ENV{PS2SDK})
    SET(PS2SDK $ENV{PS2SDK})
else()
    message(FATAL_ERROR "The environment variable PS2DEV needs to be defined.")
endif()

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_SYSTEM_PROCESSOR mips)
SET(CMAKE_C_COMPILER mips64r5900el-ps2-elf-gcc)
SET(CMAKE_CXX_COMPILER mips64r5900el-ps2-elf-g++)
SET(CMAKE_C_FLAGS_INIT   "-I${PS2SDK}/ee/include -I${PS2SDK}/common/include -I${PS2DEV}/gsKit/include -I${PS2SDK}/ports/include -D_EE -DPS2 -D__PS2__ -O2 -G0")
SET(CMAKE_CXX_FLAGS_INIT "-I${PS2SDK}/ee/include -I${PS2SDK}/common/include -I${PS2DEV}/gsKit/include -I${PS2SDK}/ports/include -D_EE -DPS2 -D__PS2__ -O2 -G0")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-T${PS2SDK}/ee/startup/linkfile -L${PS2SDK}/ee/lib -L${PS2DEV}/gsKit/lib -L${PS2SDK}/ports/lib -Wl,-zmax-page-size=128")
#SET(CMAKE_SHARED_LINKER_FLAGS_INIT "...")
#SET(CMAKE_STATIC_LINKER_FLAGS_CONFIG_INIT "...")
#SET(CMAKE_STATIC_LINKER_FLAGS_INIT "...")
SET(CMAKE_TARGET_INSTALL_PREFIX ${PS2DEV}/ports)

SET(CMAKE_FIND_ROOT_PATH ${PS2DEV} ${PS2DEV}/ee ${PS2DEV}/ee/ee ${PS2SDK} ${PS2SDK}/ports)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(PLATFORM_PS2 TRUE)
SET(PS2 TRUE)