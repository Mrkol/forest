# ==============================================================================
# Famicom — Built-in NES emulator for in-game playable NES titles (C++)
# ==============================================================================

forest_add_library(Famicom
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        ks_nes_core.cpp
        ks_nes_core_pc_stubs.cpp
        ks_nes_draw.cpp
        famicom.cpp
        famicom_nesinfo.cpp
)
