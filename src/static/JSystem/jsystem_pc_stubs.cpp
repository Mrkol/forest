/**
 * PC-only stubs for JSystem symbols that are missing on PC
 * (e.g. font data from assembly, or virtuals without out-of-line definitions).
 */
#ifdef TARGET_PC

#include "JSystem/JUtility/JUTFont.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JSupport/JSUInputStream.h"
#include "JSystem/JSupport/JSURandomInputStream.h"
#include "JSystem/JSupport/JSUStreamEnum.h"

/* Dummy system font (replaces JUTFontData_Ascfont_fix12.s on PC) */
const ResFONT JUTResFONT_Ascfont_fix12 = {};

/* JKRHeap::destroy() - instance method called by static destroy(JKRHeap*) */
void JKRHeap::destroy() {
}

/* JSUOutputStream - virtual destructor and skip (only declared in header) */
JSUOutputStream::~JSUOutputStream() {
}

int JSUOutputStream::skip(s32 amount) {
    (void)amount;
    return 0;
}

/* JSURandomOutputStream - virtuals declared but not defined in header */
int JSURandomOutputStream::getAvailable() const {
    return getLength() - getPosition();
}

int JSURandomOutputStream::skip(s32 amount) {
    s32 pos = getPosition();
    seekPos(amount, SEEK_CUR);
    return getPosition() - pos;
}

#endif /* TARGET_PC */
