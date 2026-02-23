# ==============================================================================
# libforest — Forest engine: emu64 (N64→GX display list translator), fault handler, OS report
# NOTE: emu64.c is a C file compiled as C++ in the original build
# ==============================================================================

forest_add_library(libforest
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    CXX_SOURCES
        emu64/emu64.cpp
    SOURCES
        osreport.c
        fault.c
        # ReconfigBATs.c — excluded: pure PowerPC assembly, needs PC replacement
)
