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
# PC build: embed system font via generated C source so JUTResFONT_Ascfont_fix12 is defined.
set(FONT_BFN "${CMAKE_SOURCE_DIR}/assets/JSystem/JUtility/FontData/Ascfont_fix12.bfn")
if(NOT EXISTS "${FONT_BFN}")
  message(FATAL_ERROR
    "JUtility needs the system font for the PC build.\n"
    "  Missing: ${FONT_BFN}\n"
    "  This file is produced by the decomp extraction (see config/GAFE01_00/config.yml extract).\n"
    "  Run the original configure.py/build to extract assets, or copy Ascfont_fix12.bfn into that path.")
endif()

file(READ "${FONT_BFN}" FONT_BFN_HEX HEX)
string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1, " FONT_BFN_C "${FONT_BFN_HEX}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/JUTFontData_Ascfont_fix12_pc.c"
"#include \"types.h\"\n"
"#ifdef _MSC_VER\n"
"#define FONT_ALIGN __declspec(align(32))\n"
"#else\n"
"#define FONT_ALIGN __attribute__((aligned(32)))\n"
"#endif\n"
"FONT_ALIGN const u8 JUTResFONT_Ascfont_fix12[] = {\n${FONT_BFN_C}\n};\n")
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
        ${CMAKE_CURRENT_BINARY_DIR}/JUTFontData_Ascfont_fix12_pc.c
)
