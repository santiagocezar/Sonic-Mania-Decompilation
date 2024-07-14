#!/bin/sh
export PKG_CONFIG_PATH="$PS2SDK/ports/share/pkgconfig:$PS2SDK/ports/lib/pkgconfig"
export PATH="$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin:$PS2DEV/dvp/bin:$PS2SDK/bin:$PATH"

cmake -B build . -DCMAKE_TOOLCHAIN_FILE=ps2dev.cmake -DWITH_RSDK=ON -DGAME_STATIC=ON -DRETRO_SUBSYSTEM=SDL2 -DUSE_SDL_AUDIO=ON -DPLATFORM=PlayStation2 -DRETRO_MOD_LOADER=OFF
VERBOSE=1 cmake --build build
