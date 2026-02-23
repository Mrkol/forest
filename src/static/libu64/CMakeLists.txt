# ==============================================================================
# libu64 — N64 compatibility layer: debug output, gfxprint, pad abstraction
# ==============================================================================

forest_add_library(libu64
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        debug.c
        gfxprint.c
        gfxprint_data.c
        pad.c
)
