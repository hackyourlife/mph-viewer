#include <math.h>
#include "types.h"
#include "vec.h"

void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *axb)
{
	fx32 x, y, z;

	x = (fx32)(((fx64)a->y * b->z - (fx64)a->z * b->y + (1 << (FX64_SHIFT - 1))) >> FX64_SHIFT);
	y = (fx32)(((fx64)a->z * b->x - (fx64)a->x * b->z + (1 << (FX64_SHIFT - 1))) >> FX64_SHIFT);
	z = (fx32)(((fx64)a->x * b->y - (fx64)a->y * b->x + (1 << (FX64_SHIFT - 1))) >> FX32_SHIFT);

	axb->x = x;
	axb->y = y;
	axb->z = z;
}

void VEC_Normalize(const VecFx32 *pSrc, VecFx32 *pDst)
{
	float x = FX_FX32_TO_F32(pSrc->x);
	float y = FX_FX32_TO_F32(pSrc->y);
	float z = FX_FX32_TO_F32(pSrc->z);
	float len = sqrt(x * x + y * y + z * z);
	pDst->x = FX_F32_TO_FX32(x / len);
	pDst->y = FX_F32_TO_FX32(y / len);
	pDst->z = FX_F32_TO_FX32(z / len);
}

void MTX_MultVec33(const VecFx32* vec, const MtxFx33* m, VecFx32* dst)
{
	fx32 x, y, z;

	x = vec->x;
	y = vec->y;
	z = vec->z;

	dst->x = (fx32)(((fx64)x * m->_00 + (fx64)y * m->_10 + (fx64)z * m->_20) >> FX32_SHIFT);
	dst->y = (fx32)(((fx64)x * m->_01 + (fx64)y * m->_11 + (fx64)z * m->_21) >> FX32_SHIFT);
	dst->z = (fx32)(((fx64)x * m->_02 + (fx64)y * m->_12 + (fx64)z * m->_22) >> FX32_SHIFT);
}
