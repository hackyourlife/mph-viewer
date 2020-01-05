#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#define	u8	uint8_t
#define	u16	uint16_t
#define	u32	uint32_t
#define	u64	uint64_t
#define	s8	int8_t
#define	s16	int16_t
#define	s32	int32_t
#define	s64	int64_t

#define	f32	float
#define	f64	double

typedef	char	bool;
#define	true	1
#define	false	0

#define	BOOL	int
#define	TRUE	1
#define	FALSE	0

typedef enum {
	GX_FOGSLOPE_0x8000 = 0x0,
	GX_FOGSLOPE_0x4000 = 0x1,
	GX_FOGSLOPE_0x2000 = 0x2,
	GX_FOGSLOPE_0x1000 = 0x3,
	GX_FOGSLOPE_0x0800 = 0x4,
	GX_FOGSLOPE_0x0400 = 0x5,
	GX_FOGSLOPE_0x0200 = 0x6,
	GX_FOGSLOPE_0x0100 = 0x7,
	GX_FOGSLOPE_0x0080 = 0x8,
	GX_FOGSLOPE_0x0040 = 0x9,
	GX_FOGSLOPE_0x0020 = 0xA,
} GXFogSlope;

typedef enum {
	GX_TEXFMT_NONE = 0,
	GX_TEXFMT_A3I5 = 1,
	GX_TEXFMT_PLTT4 = 2,
	GX_TEXFMT_PLTT16 = 3,
	GX_TEXFMT_PLTT256 = 4,
	GX_TEXFMT_COMP4x4 = 5,
	GX_TEXFMT_A5I3 = 6,
	GX_TEXFMT_DIRECT = 7
} GXTexFmt;

typedef enum {
	GX_TEXSIZE_S8 = 0,
	GX_TEXSIZE_S16 = 1,
	GX_TEXSIZE_S32 = 2,
	GX_TEXSIZE_S64 = 3,
	GX_TEXSIZE_S128 = 4,
	GX_TEXSIZE_S256 = 5,
	GX_TEXSIZE_S512 = 6,
	GX_TEXSIZE_S1024 = 7
} GXTexSizeS;

typedef enum {
	GX_TEXSIZE_T8 = 0,
	GX_TEXSIZE_T16 = 1,
	GX_TEXSIZE_T32 = 2,
	GX_TEXSIZE_T64 = 3,
	GX_TEXSIZE_T128 = 4,
	GX_TEXSIZE_T256 = 5,
	GX_TEXSIZE_T512 = 6,
	GX_TEXSIZE_T1024 = 7
} GXTexSizeT;

typedef enum {
	GX_TEXGEN_NONE = 0,
	GX_TEXGEN_TEXCOORD = 1,
	GX_TEXGEN_NORMAL = 2,
	GX_TEXGEN_VERTEX = 3
} GXTexGen;


typedef s16 fx16;
typedef s32 fx32;
typedef s64 fx64;
typedef s64 fx64c;

#define FX32_SHIFT		12
#define FX32_INT_SIZE		19
#define FX32_DEC_SIZE		12

#define FX32_INT_MASK		0x7ffff000
#define FX32_DEC_MASK		0x00000fff
#define FX32_SIGN_MASK		0x80000000

#define FX32_MAX		(fx32)(0x7fffffff)
#define FX32_MIN		(fx32)(0x80000000)

#define FX_MUL(v1, v2)		((fx32)(((fx64)(v1) * (v2) + 0x800LL) >> FX32_SHIFT))
#define FX_MUL32x64C(v1, v2)	((fx32)(((v2) * (v1) + 0x80000000LL) >> 32))

#define FX_FX32_TO_F32(x)	((f32)((x) / (f32)(1 << FX32_SHIFT)))
#define FX_F32_TO_FX32(x)	((fx32)(((x) > 0) ? \
			((x) * (1 << FX32_SHIFT) + 0.5f ) : \
			((x) * (1 << FX32_SHIFT) - 0.5f )))

#define FX32_CONST(x)		FX_F32_TO_FX32(x)


#define FX64_SHIFT		12
#define FX64_INT_SIZE		51
#define FX64_DEC_SIZE		12

#define FX64_INT_MASK		((fx64)0x7ffffffffffff000)
#define FX64_DEC_MASK		((fx64)0x0000000000000fff)
#define FX64_SIGN_MASK		((fx64)0x8000000000000000)

#define FX64_MAX		((fx64)0x7fffffffffffffff)
#define FX64_MIN		((fx64)0x8000000000000000)

#define FX_FX64_TO_F32(x)	((f32)((x) / (f32)(1 << FX64_SHIFT)))
#define FX_F32_TO_FX64(x)	((fx64)(((x) > 0) ? \
			((x) * (1 << FX32_SHIFT) + 0.5f ) : \
			((x) * (1 << FX32_SHIFT) - 0.5f )))

#define FX64_CONST(x)		FX_F32_TO_FX64(x)

// deg must be in fx32/fx16 format
#define FX_DEG_TO_RAD(deg)	((fx32)((FX64C_TWOPI_360 * (deg) + 0x80000000LL) >> 32))
#define FX_DEG_TO_IDX(deg)	((u16) ((FX64C_65536_360 * (deg) + 0x80000000000LL) >> 44))

// rad must be in fx32/fx16 format
#define FX_RAD_TO_DEG(rad)	((fx32)((FX64C_360_TWOPI * (rad) + 0x80000000LL) >> 32))
#define FX_RAD_TO_IDX(rad)	((u16)((FX64C_65536_TWOPI * (rad) + 0x80000000000LL) >> 44))

#define FX_IDX_TO_RAD(idx)	((fx32)((FX64C_TWOPI_65536 * (idx) + 0x80000LL) >> 20))
#define FX_IDX_TO_DEG(idx)	((fx32)((FX64C_360_65536 * (idx) + 0x80000LL) >> 20))

#include "fxconst.h"

typedef struct {
	fx32 x;
	fx32 y;
	fx32 z;
} VecFx32;

typedef union {
	struct {
		fx32 _00, _01, _02, _03;
		fx32 _10, _11, _12, _13;
		fx32 _20, _21, _22, _23;
		fx32 _30, _31, _32, _33;
	};
	fx32 m[4][4];
	fx32 a[16];
} MtxFx44;

typedef union {
	struct {
		fx32 _00, _01, _02;
		fx32 _10, _11, _12;
		fx32 _20, _21, _22;
		fx32 _30, _31, _32;
	};
	fx32 m[4][3];
	fx32 a[12];
} MtxFx43;

typedef struct
{
	u8 r;
	u8 g;
	u8 b;
} Color3;

typedef struct
{
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} Color4;

typedef struct {
	u32	unk_flag;
	char*	model_name;
	char*	anim_name;
	char*	collision_name;
	s8	anim_ids[4];
} ObjectDescription;

typedef struct {
	float x;
	float y;
	float z;
} Vec3;

typedef union {
	struct {
		float _00, _01, _02, _03;
		float _10, _11, _12, _13;
		float _20, _21, _22, _23;
		float _30, _31, _32, _33;
	};
	float m[4][4];
	float a[16];
} Mtx44;

#define MKTIME(m, s, frac) ((m) * 3600 + (s) * 60 + (frac))
#define MKCOLOR(r, g, b) ((r) | (g) << 5 | (b) << 10)
#define VECFX32(x, y, z) { FX32_CONST(x), FX32_CONST(y), FX32_CONST(z) }

#endif
