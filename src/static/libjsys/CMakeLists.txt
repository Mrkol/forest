# ==============================================================================
# libjsys — JSystem wrapper extensions (C++)
# ==============================================================================

forest_add_library(libjsys
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        jsyswrapper.cpp
)
