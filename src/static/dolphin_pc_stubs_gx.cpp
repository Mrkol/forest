/**
 * PC stubs for Dolphin GX and MTX APIs. Uses exact types from headers.
 */
#ifdef TARGET_PC

#include <dolphin/types.h>
#include <dolphin/gx/GXStruct.h>
#include <dolphin/gx/GXFrameBuffer.h>
#include <dolphin/gx/GXEnum.h>
#include <dolphin/mtx.h>
#include <dolphin/gx/GXTransform.h>
#include <dolphin/gx/GXPixel.h>
#include <dolphin/gx/GXGeometry.h>
#include <dolphin/gx/GXTev.h>
#include <dolphin/gx/GXTexture.h>
#include <dolphin/gx/GXLighting.h>
#include <dolphin/gx/GXCull.h>
#include <dolphin/gx/GXFifo.h>
#include <dolphin/gx/GXBump.h>
#include <dolphin/os/OSThread.h>

extern "C" {

/* Zero-initialized by C++ rules */
GXRenderModeObj GXNtsc480IntDf;
GXRenderModeObj GXNtsc480Int;

OSThread* GXSetCurrentGXThread(void) { return NULL; }

void GXSetAlphaUpdate(GXBool u) { (void)u; }
void GXSetPixelFmt(GXPixelFmt a, GXZFmt16 b) { (void)a;(void)b; }
void GXSetDither(GXBool d) { (void)d; }
void GXInvalidateTexAll(void) {}
void GXInvalidateVtxCache(void) {}

void C_MTXOrtho(Mtx44 mtx, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f) { (void)mtx;(void)t;(void)b;(void)l;(void)r;(void)n;(void)f; }
void PSMTXIdentity(Mtx34 mtx) { (void)mtx; }

void GXSetProjection(const void* mtx, GXProjectionType type) { (void)mtx;(void)type; }
void GXLoadPosMtxImm(const void* mtx, u32 id) { (void)mtx;(void)id; }
void GXLoadNrmMtxImm(const void* mtx, u32 id) { (void)mtx;(void)id; }
void GXLoadTexMtxImm(const void* mtx, u32 id, GXTexMtxType type) { (void)mtx;(void)id;(void)type; }
void GXSetViewport(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { (void)a;(void)b;(void)c;(void)d;(void)e;(void)f; }
void GXSetCurrentMtx(u32 id) { (void)id; }

void GXSetLineWidth(u8 w, GXTexOffset to) { (void)w;(void)to; }
void GXSetPointSize(u8 w, GXTexOffset to) { (void)w;(void)to; }
void GXClearVtxDesc(void) {}
void GXSetNumTexGens(u8 n) { (void)n; }
void GXEnableTexOffsets(GXTexCoordID c, GXBool line, GXBool point) { (void)c;(void)line;(void)point; }

void GXInitTexObj(GXTexObj* o, void* img, u16 w, u16 h, GXTexFmt fmt, GXTexWrapMode ws, GXTexWrapMode wt, u8 mip) { (void)o;(void)img;(void)w;(void)h;(void)fmt;(void)ws;(void)wt;(void)mip; }
void GXLoadTexObj(GXTexObj* o, GXTexMapID id) { (void)o;(void)id; }

void GXSetNumChans(u8 n) { (void)n; }
void GXSetChanCtrl(GXChannelID c, GXBool e, GXColorSrc as, GXColorSrc ms, u32 lm, GXDiffuseFn df, GXAttnFn af) { (void)c;(void)e;(void)as;(void)ms;(void)lm;(void)df;(void)af; }
void GXSetChanAmbColor(GXChannelID c, GXColor ac) { (void)c;(void)ac; }
void GXSetChanMatColor(GXChannelID c, GXColor mc) { (void)c;(void)mc; }
void GXSetTexCoordGen2(GXTexCoordID id, GXTexGenType type, GXTexGenSrc src, u32 mtx, GXBool norm, u32 post) { (void)id;(void)type;(void)src;(void)mtx;(void)norm;(void)post; }

void GXSetNumTevStages(u8 n) { (void)n; }
void GXSetTevColorIn(GXTevStageID s, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c, GXTevColorArg d) { (void)s;(void)a;(void)b;(void)c;(void)d; }
void GXSetTevAlphaIn(GXTevStageID s, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c, GXTevAlphaArg d) { (void)s;(void)a;(void)b;(void)c;(void)d; }
void GXSetTevOrder(GXTevStageID s, GXTexCoordID coord, GXTexMapID map, GXChannelID col) { (void)s;(void)coord;(void)map;(void)col; }
void GXSetTevColorOp(GXTevStageID s, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp, GXTevRegID out) { (void)s;(void)op;(void)bias;(void)scale;(void)clamp;(void)out; }
void GXSetTevAlphaOp(GXTevStageID s, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp, GXTevRegID out) { (void)s;(void)op;(void)bias;(void)scale;(void)clamp;(void)out; }
void GXSetTevOp(GXTevStageID id, GXTevMode mode) { (void)id;(void)mode; }
void GXSetAlphaCompare(GXCompare c0, u8 r0, GXAlphaOp op, GXCompare c1, u8 r1) { (void)c0;(void)r0;(void)op;(void)c1;(void)r1; }
void GXSetZTexture(GXZTexOp op, GXTexFmt fmt, u32 bias) { (void)op;(void)fmt;(void)bias; }
void GXSetTevKColorSel(GXTevStageID s, GXTevKColorSel sel) { (void)s;(void)sel; }
void GXSetTevKAlphaSel(GXTevStageID s, GXTevKAlphaSel sel) { (void)s;(void)sel; }
void GXSetTevSwapMode(GXTevStageID s, GXTevSwapSel ras, GXTevSwapSel tex) { (void)s;(void)ras;(void)tex; }
void GXSetTevSwapModeTable(GXTevSwapSel t, GXTevColorChan r, GXTevColorChan g, GXTevColorChan b, GXTevColorChan a) { (void)t;(void)r;(void)g;(void)b;(void)a; }
void GXSetTevDirect(GXTevStageID s) { (void)s; }

void GXSetNumIndStages(u8 n) { (void)n; }
void GXSetIndTexCoordScale(GXIndTexStageID s, GXIndTexScale ss, GXIndTexScale st) { (void)s;(void)ss;(void)st; }

void GXSetFog(GXFogType type, f32 start, f32 end, f32 nearz, f32 farz, GXColor color) { (void)type;(void)start;(void)end;(void)nearz;(void)farz;(void)color; }
void GXSetFogRangeAdj(GXBool enable, u16 center, GXFogAdjTable* table) { (void)enable;(void)center;(void)table; }
void GXSetBlendMode(GXBlendMode t, GXBlendFactor sf, GXBlendFactor df, GXLogicOp op) { (void)t;(void)sf;(void)df;(void)op; }
void GXSetColorUpdate(GXBool e) { (void)e; }
void GXSetZMode(GXBool e, GXCompare f, GXBool u) { (void)e;(void)f;(void)u; }
void GXSetZCompLoc(GXBool b) { (void)b; }
void GXSetDstAlpha(GXBool e, u8 a) { (void)e;(void)a; }
void GXSetFieldMask(GXBool odd, GXBool even) { (void)odd;(void)even; }
void GXSetFieldMode(GXBool fm, GXBool half) { (void)fm;(void)half; }

void GXSetCoPlanar(GXBool e) { (void)e; }
void GXSetCullMode(GXCullMode m) { (void)m; }
void GXSetClipMode(GXClipMode m) { (void)m; }
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht) { (void)left;(void)top;(void)wd;(void)ht; }
void GXSetScissorBoxOffset(s32 x, s32 y) { (void)x;(void)y; }

/* MTX (additional) */
void PSMTXCopy(const Mtx src, Mtx dest) { (void)src;(void)dest; }
void PSMTXScale(Mtx mtx, f32 x, f32 y, f32 z) { (void)mtx;(void)x;(void)y;(void)z; }
void PSMTXTrans(Mtx mtx, f32 x, f32 y, f32 z) { (void)mtx;(void)x;(void)y;(void)z; }
void PSMTXConcat(const Mtx a, const Mtx b, Mtx out) { (void)a;(void)b;(void)out; }
void PSMTXMultVec(const Mtx m, const Vec* in, Vec* out) { (void)m;(void)in;(void)out; }
u32 PSMTXInverse(const Mtx src, Mtx inv) { (void)src;(void)inv; return 0; }
void PSVECNormalize(const Vec* src, Vec* dst) { (void)src;(void)dst; }
void C_MTXLightOrtho(Mtx mtx, f32 t, f32 b, f32 l, f32 r, f32 scaleS, f32 scaleT, f32 transS, f32 transT) { (void)mtx;(void)t;(void)b;(void)l;(void)r;(void)scaleS;(void)scaleT;(void)transS;(void)transT; }

/* GX Texture (additional) */
void GXInitTexObjCI(GXTexObj* o, void* img, u16 w, u16 h, GXCITexFmt fmt, GXTexWrapMode ws, GXTexWrapMode wt, u8 mip, u32 tlut) { (void)o;(void)img;(void)w;(void)h;(void)fmt;(void)ws;(void)wt;(void)mip;(void)tlut; }
void GXInitTexObjLOD(GXTexObj* o, GXTexFilter minF, GXTexFilter magF, f32 minL, f32 maxL, f32 bias, GXBool biasClamp, GXBool edgeLod, GXAnisotropy aniso) { (void)o;(void)minF;(void)magF;(void)minL;(void)maxL;(void)bias;(void)biasClamp;(void)edgeLod;(void)aniso; }
void GXInitTlutObj(GXTlutObj* o, void* lut, GXTlutFmt fmt, u16 n) { (void)o;(void)lut;(void)fmt;(void)n; }
void GXLoadTlut(GXTlutObj* o, u32 idx) { (void)o;(void)idx; }

/* GX Tev */
void GXSetTevColor(GXTevRegID id, GXColor c) { (void)id;(void)c; }

/* GX Vert (TARGET_PC non-inline) */
void GXPosition3f32(f32 x, f32 y, f32 z) { (void)x;(void)y;(void)z; }
void GXPosition2f32(f32 x, f32 y) { (void)x;(void)y; }
void GXPosition2u16(u16 x, u16 y) { (void)x;(void)y; }
void GXNormal3f32(f32 x, f32 y, f32 z) { (void)x;(void)y;(void)z; }
void GXColor1u32(u32 c) { (void)c; }
void GXTexCoord2s16(s16 s, s16 t) { (void)s;(void)t; }
void GXTexCoord2f32(f32 s, f32 t) { (void)s;(void)t; }
void GXTexCoord2u8(u8 s, u8 t) { (void)s;(void)t; }
void GXEnd(void) {}

/* GX Geometry */
void GXSetVtxDesc(GXAttr attr, GXAttrType type) { (void)attr;(void)type; }
void GXSetVtxAttrFmt(GXVtxFmt fmt, GXAttr attr, GXCompCnt cnt, GXCompType type, u8 frac) { (void)fmt;(void)attr;(void)cnt;(void)type;(void)frac; }
void GXBegin(GXPrimitive type, GXVtxFmt fmt, u16 nverts) { (void)type;(void)fmt;(void)nverts; }

/* GX Light */
void GXInitLightAttn(GXLightObj* o, f32 a0, f32 a1, f32 a2, f32 k0, f32 k1, f32 k2) { (void)o;(void)a0;(void)a1;(void)a2;(void)k0;(void)k1;(void)k2; }
void GXInitLightColor(GXLightObj* o, GXColor c) { (void)o;(void)c; }
void GXInitLightDir(GXLightObj* o, f32 x, f32 y, f32 z) { (void)o;(void)x;(void)y;(void)z; }
void GXInitLightPos(GXLightObj* o, f32 x, f32 y, f32 z) { (void)o;(void)x;(void)y;(void)z; }
void GXLoadLightObjImm(GXLightObj* o, GXLightID id) { (void)o;(void)id; }

/* GX Fifo / Misc */
void GXInit(void) {}
void GXInitFifoBase(GXFifoObj* fifo, void* base, u32 size) { (void)fifo;(void)base;(void)size; }
void GXInitFifoPtrs(GXFifoObj* fifo, void* readPtr, void* writePtr) { (void)fifo;(void)readPtr;(void)writePtr; }
void GXSaveCPUFifo(GXFifoObj* fifo) { (void)fifo; }
OSThread* GXGetCurrentGXThread(void) { return NULL; }
void GXGetGPStatus(GXBool* a, GXBool* b, GXBool* c, GXBool* d, GXBool* e) { (void)a;(void)b;(void)c;(void)d;(void)e; }
void GXFlush(void) {}
void GXDrawDone(void) {}
void GXAbortFrame(void) {}
void GXCallDisplayList(const void* list, u32 nbytes) { (void)list;(void)nbytes; }
void GXPixModeSync(void) {}
void GXReadXfRasMetric(u32* a, u32* b, u32* c, u32* d) { (void)a;(void)b;(void)c;(void)d; }
void GXSetDrawDone(void) {}
typedef void (*GXDrawDoneCallback)(void);
void GXSetDrawDoneCallback(GXDrawDoneCallback cb) { (void)cb; }

/* GX FrameBuffer */
void GXSetCopyClear(GXColor c, u32 z) { (void)c;(void)z; }
void GXSetCopyClamp(GXFBClamp c) { (void)c; }
void GXSetCopyFilter(GXBool aa, const u8 (*sample)[2], GXBool vf, const u8* vfilter) { (void)aa;(void)sample;(void)vf;(void)vfilter; }
void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht) { (void)left;(void)top;(void)wd;(void)ht; }
void GXSetDispCopyDst(u16 wd, u16 ht) { (void)wd;(void)ht; }
void GXSetDispCopyGamma(GXGamma g) { (void)g; }
u32 GXSetDispCopyYScale(f32 s) { (void)s; return 0; }
void GXCopyDisp(void* dest, GXBool clear) { (void)dest;(void)clear; }

} /* extern "C" */

#endif
