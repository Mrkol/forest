# ==============================================================================
# JSystem Libraries (C++)
# ==============================================================================

# --- J2DGraph ---
forest_add_library(J2DGraph
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        J2DGraph/J2DGrafContext.cpp
        J2DGraph/J2DOrthoGraph.cpp
)

# --- JFramework ---
forest_add_library(JFramework
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        JFramework/JFWDisplay.cpp
        JFramework/JFWSystem.cpp
        jsystem_pc_stubs.cpp
)

# --- JGadget ---
forest_add_library(JGadget
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        JGadget/linklist.cpp
)

# --- JKernel ---
forest_add_library(JKernel
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        JKernel/JKRAram.cpp
        JKernel/JKRAramArchive.cpp
        JKernel/JKRAramBlock.cpp
        JKernel/JKRAramHeap.cpp
        JKernel/JKRAramPiece.cpp
        JKernel/JKRAramStream.cpp
        JKernel/JKRArchivePri.cpp
        JKernel/JKRArchivePub.cpp
        JKernel/JKRCompArchive.cpp
        JKernel/JKRDecomp.cpp
        JKernel/JKRDisposer.cpp
        JKernel/JKRDvdAramRipper.cpp
        JKernel/JKRDvdArchive.cpp
        JKernel/JKRDvdFile.cpp
        JKernel/JKRDvdRipper.cpp
        JKernel/JKRExpHeap.cpp
        JKernel/JKRFileFinder.cpp
        JKernel/JKRFileLoader.cpp
        JKernel/JKRHeap.cpp
        JKernel/JKRMemArchive.cpp
        JKernel/JKRThread.cpp
)

# --- JSupport ---
forest_add_library(JSupport
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        JSupport/JSUFileStream.cpp
        JSupport/JSUInputStream.cpp
        JSupport/JSUList.cpp
)

# --- JUtility ---
# NOTE: JUTFontData_Ascfont_fix12.s is PowerPC assembly and is excluded.
forest_add_library(JUtility
    BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}
    INTERFACE_TARGET forest_static
    SOURCES
        JUtility/JUTAssert.cpp
        JUtility/JUTConsole.cpp
        JUtility/JUTDbPrint.cpp
        JUtility/JUTDirectFile.cpp
        JUtility/JUTDirectPrint.cpp
        JUtility/JUTException.cpp
        JUtility/JUTFader.cpp
        JUtility/JUTFont.cpp
        JUtility/JUTGamePad.cpp
        JUtility/JUTGraphFifo.cpp
        JUtility/JUTProcBar.cpp
        JUtility/JUTResFont.cpp
        JUtility/JUTVideo.cpp
        JUtility/JUTXfb.cpp
)
