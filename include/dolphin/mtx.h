#ifndef MTX_H
#define MTX_H

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MTX_PS

/////////////// TYPE DEFINES ///////////////
#define MTXDegToRad(a) ((a) * 0.01745329252f)

typedef struct {
    f32 x, y, z;
} Vec, *VecPtr, Point3d, *Point3dPtr;

typedef f32 Mtx34[3][4];
typedef f32 Mtx23[2][3];
typedef f32 Mtx33[3][3];
typedef f32 Mtx44[4][4];
typedef f32 (*MtxPtr)[4];
typedef f32 PSQuaternion[4];
typedef Mtx34 Mtx;

typedef struct Quaternion {
    f32 x, y, z, w;
} Quaternion;

////////////////////////////////////////////

////// PAIRED SINGLE MATRIX FUNCTIONS //////
void PSMTXIdentity(Mtx mtx);
void PSMTXCopy(const Mtx src, Mtx dest);
void PSMTXConcat(const Mtx A, const Mtx B, Mtx concat);

void PSMTXTranspose(const Mtx src, Mtx xPose);
u32 PSMTXInverse(const Mtx src, Mtx inv);

void __PSMTXRotAxisRadInternal(Mtx mtx, const Vec* axis, f32 sinA, f32 cosA);
void PSMTXRotRad(Mtx mtx, char axis, f32 angle);
void PSMTXRotTrig(Mtx mtx, char axis, f32 sinA, f32 cosA);
void PSMTXRotAxisRad(Mtx mtx, const Vec* axis, f32 angle);

void PSMTXTrans(Mtx mtx, f32 xT, f32 yT, f32 zT);
void PSMTXTransApply(const Mtx src, Mtx dest, f32 xT, f32 yT, f32 zT);

void PSMTXScale(Mtx mtx, f32 xS, f32 yS, f32 zS);
void PSMTXScaleApply(const Mtx src, Mtx dest, f32 xS, f32 yS, f32 zS);
void PSMTXQuat(Mtx mtx, const PSQuaternion* quat);

void PSVECNormalize(const Vec* src, Vec* dst);
void PSMTXMultVec(const Mtx m, const Vec* src, Vec* dst);

////////////////////////////////////////////

//// PAIRED SINGLE MATRIX VEC FUNCTIONS ////
void PSMTXMultVec(const Mtx, const Vec*, Vec*);
void PSMTXMultVecSR(const Mtx, const Vec*, Vec*);
void PSMTXMultVecArraySR(const Mtx, f32*, f32*, f32*);

////////////////////////////////////////////

///////////  MATRIX44 FUNCTIONS ////////////
void PSMTX44Copy(Mtx44 src, Mtx44 dest);
void C_MTXPerspective(Mtx44 mtx, f32 fovY, f32 aspect, f32 n, f32 f);
void C_MTXOrtho(Mtx44 mtx, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
////////////////////////////////////////////

///////// CODED C MATRIX FUNCTIONS /////////
void C_MTXLookAt(Mtx, const Vec*, const Vec*, const Vec*);
void C_MTXLightPerspective(Mtx mtx, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
void C_MTXLightOrtho(Mtx mtx, f32 t, f32 b, f32 l, f32 r, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
////////////////////////////////////////////

////////////// MATRIX INLINES //////////////
static inline void MTXSetPosition(Mtx mtx, const Vec* pos) {
    mtx[0][3] = pos->x;
    mtx[1][3] = pos->y;
    mtx[2][3] = pos->z;
}

////////////////////////////////////////////

#ifdef MTX_PS
#define VECSquareMag PSVECSquareMag
#define VECNormalize PSVECNormalize
#define VECDistance PSVECDistance
#define VECMag PSVECMag
#define VECAdd PSVECAdd
#define VECDotProduct PSVECDotProduct
#define VECSquareDistance PSVECSquareDistance
#define VECCrossProduct PSVECCrossProduct
#define MTXMultVec PSMTXMultVec
#define MTXMultVecArray PSMTXMultVecArray
#define MTXMultVecSR PSMTXMultVecSR
#define MTXScale PSMTXScale
#define MTXCopy PSMTXCopy
#define MTXConcat PSMTXConcat
#define MTXInverse PSMTXInverse
#define MTXTranspose PSMTXTranspose
#define MTXIdentity PSMTXIdentity
#define MTXRotRad PSMTXRotRad
#define MTXTrans PSMTXTrans
#else
#define VECSquareMag C_VECSquareMag
#define VECNormalize C_VECNormalize
#define VECDistance C_VECDistance
#define VECMag C_VECMag
#define VECAdd C_VECAdd
#define VECDotProduct C_VECDotProduct
#define VECSquareDistance C_VECSquareDistance
#define VECCrossProduct C_VECCrossProduct
#define MTXMultVec C_MTXMultVec
#define MTXMultVecArray C_MTXMultVecArray
#define MTXMultVecSR C_MTXMultVecSR
#define MTXScale C_MTXScale
#define MTXCopy C_MTXCopy
#define MTXConcat C_MTXConcat
#define MTXInverse C_MTXInverse
#define MTXTranspose C_MTXTranspose
#define MTXIdentity C_MTXIdentity
#define MTXRotRad C_MTXRotRad
#define MTXTrans C_MTXTrans
#endif

#ifdef __cplusplus
}
#endif

#endif
