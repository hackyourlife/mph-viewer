#ifndef __VEC_H__
#define __VEC_H__

#include "types.h"

void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *axb);
void VEC_CrossProduct3(const Vec3 *a, const Vec3 *b, Vec3 *axb);
void VEC_Normalize(const VecFx32 *src, VecFx32 *dst);
void VEC_Normalize3(const Vec3 *src, Vec3 *dst);
void MTX_MultVec33(const VecFx32* vec, const MtxFx33* m, VecFx32* dst);

#endif
