# ==============================================================================
# Static DOL Libraries (non-Dolphin SDK)
# ==============================================================================
# NOTE: The following are EXCLUDED from this build:
#   - Dolphin SDK (src/static/dolphin/*) — GC hardware API, needs PC replacements
#   - MSL_C.PPCEABI.bare.H — Metrowerks C runtime, replaced by system libc/libm
#   - Runtime.PPCEABI.H — PowerPC EABI runtime support
#   - TRK_MINNOW_DOLPHIN — GC debugger kernel
# These are all GameCube/PowerPC-specific and will be replaced with PC equivalents.
# ==============================================================================

# --- boot ---
# System bootstrap, initialization, DVD error handling, logo data
# Boot sources live directly in src/static/ (no dedicated subfolder)
forest_add_library(boot
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        boot.c
        version.c
        initial_menu.c
        dvderr.c
        bootdata/gam_win1.c
        bootdata/gam_win2.c
        bootdata/gam_win3.c
        bootdata/logo_nin.c
        nintendo_hi_0.c
        GBA2/JoyBoot.c
    CXX_SOURCES
        jsyswrap.cpp
)

add_subdirectory(libforest)
add_subdirectory(libu64)
add_subdirectory(libc64)
add_subdirectory(libultra)
add_subdirectory(libjsys)
add_subdirectory(JSystem)
add_subdirectory(jaudio_NES)
add_subdirectory(Famicom)
