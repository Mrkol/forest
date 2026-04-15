#include "libultra/gu.h"

void guMtxF2L(float mf[4][4], Mtx *m)
{
	int	i, j;
	int	e1,e2;
	int	*ai,*af;
	u32 ue1, ue2;


	ai=(int *) &m->m[0][0];
	af=(int *) &m->m[2][0];

	for (i=0; i<4; i++)
	for (j=0; j<2; j++) {
		e1=FTOFIX32(mf[i][j*2]);
		e2=FTOFIX32(mf[i][j*2+1]);
		ue1 = (u32)e1;
		ue2 = (u32)e2;
		*(ai++) = (int)((ue2 & 0xffff0000u) | (ue1 >> 16));
		*(af++) = (int)((ue2 << 16) | (ue1 & 0xffffu));
	}
}

void guMtxIdentF(float mf[4][4])
{
	int	i, j;

	for (i=0; i<4; i++)
	for (j=0; j<4; j++)
		if (i == j) mf[i][j] = 1.0;
		else mf[i][j] = 0.0;
}


void guMtxIdent(Mtx *m)
{
        float	mf[4][4];

	guMtxIdentF(mf);

	guMtxF2L(mf, m);
}

