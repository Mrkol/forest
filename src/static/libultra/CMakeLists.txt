# ==============================================================================
# libultra — N64 Ultra SDK reimplementation: threads, messages, timers, controllers, GU math
# ==============================================================================

forest_add_library(libultra
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        contquery.c
        contreaddata.c
        controller.c
        contsetch.c
        gu/coss.c
        createmesgqueue.c
        createthread.c
        destroythread.c
        getthreadid.c
        gettime.c
        initialize.c
        gu/lookat.c
        gu/lookathil.c
        gu/mtxutil.c
        gu/normalize.c
        gu/ortho.c
        gu/perspective.c
        recvmesg.c
        gu/rotate.c
        gu/scale.c
        sendmesg.c
        setthreadpri.c
        settimer.c
        shutdown.c
        gu/sins.c
        startthread.c
        stoptimer.c
        timerintr.c
        gu/translate.c
        ultra.c
        xldtob.c
        xlitob.c
        xprintf.c
)
