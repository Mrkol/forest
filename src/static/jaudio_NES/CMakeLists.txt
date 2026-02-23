# ==============================================================================
# jaudio_NES — NES audio system
# All .c files in this library are compiled as C++ (matching the original build)
# ==============================================================================

# --- jaudio_NES_game ---
# Game-side audio interface
forest_add_library(jaudio_NES_game
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    CXX_SOURCES
        game/audioheaders.c
        game/dummyprobe.c
        game/emusound.c
        game/game64.c
        game/kappa.c
        game/melody.c
        game/radio.c
        game/rhythm.c
        game/staff.c
        game/verysimple.c
)

# --- jaudio_NES_internal ---
# Audio engine internals
forest_add_library(jaudio_NES_internal
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    CXX_SOURCES
        internal/aictrl.c
        internal/astest.c
        internal/audiothread.c
        internal/cpubuf.c
        internal/dspboot.c
        internal/dspbuf.c
        internal/dspproc.c
        internal/dummyrom.c
        internal/dvdthread.c
        internal/neosthread.c
        internal/os.c
        internal/playercall.c
        internal/random.c
        internal/sample.c
        internal/streamctrl.c
        internal/sub_sys.c
        internal/track.c
        internal/aramcall.c
        internal/audioconst.c
        internal/audiotable.c
        internal/audiowork.c
        internal/bankdrv.c
        internal/bankread.c
        internal/centcalc.c
        internal/channel.c
        internal/cmdstack.c
        internal/connect.c
        internal/driver.c
        internal/driverinterface.c
        internal/dsp_cardunlock.c
        internal/dsp_GBAKey.c
        internal/dspdriver.c
        internal/dspinterface.c
        internal/effect.c
        internal/fat.c
        internal/fxinterface.c
        internal/heapctrl.c
        internal/ipldec.c
        internal/ja_calc.c
        internal/jammain_2.c
        internal/jamosc.c
        internal/memory.c
        internal/midplay.c
        internal/noteon.c
        internal/oneshot.c
        internal/rate.c
        internal/rspsim.c
        internal/seqsetup.c
        internal/system.c
        internal/tables.c
        internal/waveread.c
)
