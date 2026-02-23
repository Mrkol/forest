# ==============================================================================
# libc64 — C utility library: malloc wrappers, math, printf, random
# ==============================================================================

forest_add_library(libc64
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        __osMalloc.c
        aprintf.c
        math64.c
        qrand.c
        sleep.c
        sprintf.c
        malloc.c
)
