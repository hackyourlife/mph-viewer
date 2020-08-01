#include <string.h>
#include <math.h>

#include <GL/gl.h>

#include "types.h"
#include "io.h"
#include "animation.h"
#include "heap.h"
#include "endianess.h"
#include "model.h"

typedef struct {
	u32	frame_count;
	u32	color_lut; // u8*
	u32	anim_count;
	u32	animations; // MaterialAnimation*
	u16	anim_frame;
	u16	field_12;
	u32	field_14;
	u32	field_18;
	u32	field_1C;
	u32	field_20;
	u32	field_24;
	u32	field_28;
	u32	field_2C;
	u32	field_30;
	u32	field_34;
	u32	field_38;
	u32	field_3C;
	u32	field_40;
	u32	field_44;
	u32	field_48;
	u32	field_4C;
	u32	field_50;
	u32	field_54;
	u32	field_58;
	u32	field_5C;
	u32	field_60;
	u32	field_64;
	u32	field_68;
	u32	field_6C;
	u32	field_70;
	u32	field_74;
	u32	field_78;
	u32	field_7C;
	u32	field_80;
	u32	field_84;
	u32	field_88;
} MaterialAnimationGroup;

typedef struct {
	u16	frame_count;
	u16	frame_index_count;
	u16	texid_count;
	u16	palid_count;
	u16	anim_count;
	u16	field_A;
	u32	frame_indices; // u16*
	u32	texids; // u16*
	u32	palids; // u16*
	u32	animations; // TextureAnimation*
	u16	anim_frame;
	u16	field_1E;
} TextureAnimationGroup;

typedef struct {
	u32	frame_count;
	u32	scale_lut; // fx32*
	u32	rot_lut; // u16*
	u32	translate_lut; // fx32*
	u32	anim_count;
	u32	animations; // TexcoordAnimation*
	u16	anim_frame;
	u16	field_1A;
} TexcoordAnimationGroup;

typedef struct {
	u32	frame_count;
	u32	fx32_ptr; // u32*
	u32	short_ptr; // u16*
	u32	int_ptr; // u32*
	u32*	node_animations; // NodeAnimation*
} NodeAnimationGroup;

typedef struct {
	u32	node_animations; // NodeAnimationGroup**
	u32	field_4; // void*
	u32	material_animations; // MaterialAnimationGroup**
	u32	texcoord_animations; // TexcoordAnimationGroup**
	u32	texture_animations; // TextureAnimationGroup**
	u16	count;
	u16	field_16;
} Animation;

typedef struct {
	char	material_name[64];
	u32	field_40;
	u8	diffuse_r_blend;
	u8	diffuse_g_blend;
	u8	diffuse_b_blend;
	u8	field_47;
	u16	diffuse_r_lut_len;
	u16	diffuse_g_lut_len;
	u16	diffuse_b_lut_len;
	u16	diffuse_r_lut_idx;
	u16	diffuse_g_lut_idx;
	u16	diffuse_b_lut_idx;
	u8	ambient_r_blend;
	u8	ambient_g_blend;
	u8	ambient_b_blend;
	u8	field_57;
	u16	ambient_r_lut_len;
	u16	ambient_g_lut_len;
	u16	ambient_b_lut_len;
	u16	ambient_r_lut_idx;
	u16	ambient_g_lut_idx;
	u16	ambient_b_lut_idx;
	u8	specular_r_blend;
	u8	specular_g_blend;
	u8	specular_b_blend;
	u8	field_67;
	u16	specular_r_lut_len;
	u16	specular_g_lut_len;
	u16	specular_b_lut_len;
	u16	specular_r_lut_idx;
	u16	specular_g_lut_idx;
	u16	specular_b_lut_idx;
	u32	field_74;
	u32	field_78;
	u32	field_7C;
	u32	field_80;
	u8	alpha_blend;
	u8	field_85;
	u16	alpha_lut_len;
	u16	alpha_lut_idx;
	u16	material_id;
} MaterialAnimation;

typedef struct {
	char	material_name[32];
	u16	count;
	u16	start_idx;
	u16	min_palid;
	u16	material_id;
	u16	min_texid;
	u16	field_2A;
} TextureAnimation;

typedef struct {
	char	material_name[32];
	u8	scale_s_blend;
	u8	scale_t_blend;
	u16	scale_s_lut_len;
	u16	scale_t_lut_len;
	u16	scale_s_lut_idx;
	u16	scale_t_lut_idx;
	u8	rot_z_blend;
	u8	field_2B;
	u16	rot_z_lut_len;
	u16	rot_z_lut_idx;
	u8	translate_s_blend;
	u8	translate_t_blend;
	u16	translate_s_lut_len;
	u16	translate_t_lut_len;
	u16	translate_s_lut_idx;
	u16	translate_t_lut_idx;
	u16	field_3A;
} TexcoordAnimation;

typedef struct {
	u8	field_0;
	u8	field_1;
	u8	field_2;
	u8	flags;
	u16	field_4;
	u16	field_6;
	u16	field_8;
	u16	field_A;
	u16	field_C;
	u16	field_E;
	u8	field_10;
	u8	field_11;
	u8	field_12;
	u8	field_13;
	u16	field_14;
	u16	field_16;
	u16	field_18;
	u16	field_1A;
	u16	field_1C;
	u16	field_1E;
	u8	field_20;
	u8	field_21;
	u8	field_22;
	u8	field_23;
	u16	field_24;
	u16	field_26;
	u16	field_28;
	u16	field_2A;
	u16	field_2C;
	u16	field_2E;
} NodeAnimation;

CAnimation* parse_animation(Animation *animation, CModel *model);
void parse_texcoord_animation(CModel* model, CTexcoordAnimationGroup* animation_group);
void parse_material_animation(CModel* model, CMaterialAnimationGroup* animation_group);

void load_animation(CAnimation** animation, const char* filename, CModel* model, char flags)
{
	Animation* raw;
	if(flags & USE_ARCHIVE)
		LoadFileFromArchive((void **)&raw, filename);
	else
		LoadFile((void **)&raw, filename);

	printf("loading animation %s\n", filename);
	*animation = parse_animation(raw, model);

	free_to_heap(raw);
}

CAnimation* parse_animation(Animation* animation, CModel* model)
{
	unsigned int i;

	CAnimation* anim = (CAnimation*) alloc_from_heap(sizeof(CAnimation));
	memset(anim, 0, sizeof(CAnimation));

	u32*	node_animations		= (u32*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&animation->node_animations));
	u32*	material_animations	= (u32*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&animation->material_animations));
	u32*	texcoord_animations	= (u32*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&animation->texcoord_animations));
	u32*	texture_animations	= (u32*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&animation->texture_animations));

	anim->count = get16bit_LE((u8*)&animation->count);

	printf("%d animation groups\n", anim->count);

	anim->texcoord_animations = (CTexcoordAnimationGroup**) alloc_from_heap(anim->count * sizeof(CTexcoordAnimationGroup*));
	anim->material_animations = (CMaterialAnimationGroup**) alloc_from_heap(anim->count * sizeof(CMaterialAnimationGroup*));
	for(i = 0; i < anim->count; i++) {
		if(texcoord_animations[i]) {
			int maxscale = 0;
			int maxrot = 0;
			int maxxlat = 0;
			int j;

			TexcoordAnimationGroup* raw_texcoord_anim_group = (TexcoordAnimationGroup*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&texcoord_animations[i]));
			TexcoordAnimation* raw_texcoord_anims = (TexcoordAnimation*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&raw_texcoord_anim_group->animations));
			CTexcoordAnimationGroup* texcoord_anims = (CTexcoordAnimationGroup*) alloc_from_heap(sizeof(CTexcoordAnimationGroup));

			fx32* scales = (fx32*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&raw_texcoord_anim_group->scale_lut));
			u16* rotations = (u16*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&raw_texcoord_anim_group->rot_lut));
			fx32* translations = (fx32*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&raw_texcoord_anim_group->translate_lut));

			anim->texcoord_animations[i] = texcoord_anims;

			texcoord_anims->frame_count = get32bit_LE((u8*)&raw_texcoord_anim_group->frame_count);
			texcoord_anims->current_frame = get16bit_LE((u8*)&raw_texcoord_anim_group->anim_frame);
			texcoord_anims->count = get16bit_LE((u8*)&raw_texcoord_anim_group->anim_count);
			texcoord_anims->animations = (CTexcoordAnimation*) alloc_from_heap(texcoord_anims->count * sizeof(CTexcoordAnimation));

#define UPDATE_MAXFRAME(max, x) { \
	if((x) > max) { \
		max = (x); \
	} \
}

			for(j = 0; j < texcoord_anims->count; j++) {
				TexcoordAnimation* raw_anim = &raw_texcoord_anims[j];
				CTexcoordAnimation* animation = &texcoord_anims->animations[j];

				memcpy(animation->material_name, raw_anim->material_name, 64);
				animation->scale_s_blend = raw_anim->scale_s_blend;
				animation->scale_t_blend = raw_anim->scale_t_blend;
				animation->scale_s_len = get16bit_LE((u8*)&raw_anim->scale_s_lut_len);
				animation->scale_t_len = get16bit_LE((u8*)&raw_anim->scale_t_lut_len);
				animation->scale_s_idx = get16bit_LE((u8*)&raw_anim->scale_s_lut_idx);
				animation->scale_t_idx = get16bit_LE((u8*)&raw_anim->scale_t_lut_idx);
				animation->rot_blend = raw_anim->rot_z_blend;
				animation->rot_len = get16bit_LE((u8*)&raw_anim->rot_z_lut_len);
				animation->rot_idx = get16bit_LE((u8*)&raw_anim->rot_z_lut_idx);
				animation->translate_s_blend = raw_anim->translate_s_blend;
				animation->translate_t_blend = raw_anim->translate_t_blend;
				animation->translate_s_len = get16bit_LE((u8*)&raw_anim->translate_s_lut_len);
				animation->translate_t_len = get16bit_LE((u8*)&raw_anim->translate_t_lut_len);
				animation->translate_s_idx = get16bit_LE((u8*)&raw_anim->translate_s_lut_idx);
				animation->translate_t_idx = get16bit_LE((u8*)&raw_anim->translate_t_lut_idx);

				UPDATE_MAXFRAME(maxscale, animation->scale_s_idx + animation->scale_s_len);
				UPDATE_MAXFRAME(maxscale, animation->scale_t_idx + animation->scale_t_len);
				UPDATE_MAXFRAME(maxrot, animation->rot_idx + animation->rot_len);
				UPDATE_MAXFRAME(maxxlat, animation->translate_s_idx + animation->translate_s_len);
				UPDATE_MAXFRAME(maxxlat, animation->translate_t_idx + animation->translate_t_len);
			}

			printf("[%d] texcoord animation group with %d/%d/%d frames\n", i, maxscale, maxrot, maxxlat);

			texcoord_anims->scales = (float*) alloc_from_heap(maxscale * sizeof(float));
			texcoord_anims->rotations = (float*) alloc_from_heap(maxrot * sizeof(float));
			texcoord_anims->translations = (float*) alloc_from_heap(maxxlat * sizeof(float));

			for(j = 0; j < maxscale; j++)
				texcoord_anims->scales[j] = FX_FX32_TO_F32(scales[j]);

			for(j = 0; j < maxrot; j++)
				texcoord_anims->rotations[j] = FX_FX32_TO_F32(FX_IDX_TO_RAD(rotations[j]));

			for(j = 0; j < maxxlat; j++)
				texcoord_anims->translations[j] = FX_FX32_TO_F32(translations[j]);

			texcoord_anims->time = 0;
		} else {
			anim->texcoord_animations[i] = NULL;
		}

		if(material_animations[i]) {
			int maxcolor = 0;
			int j;

			MaterialAnimationGroup* raw_material_anim_group = (MaterialAnimationGroup*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&material_animations[i]));
			MaterialAnimation* raw_material_anims = (MaterialAnimation*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&raw_material_anim_group->animations));
			CMaterialAnimationGroup* material_anims = (CMaterialAnimationGroup*) alloc_from_heap(sizeof(CMaterialAnimationGroup));

			anim->material_animations[i] = material_anims;

			u8* color_lut = (u8*) ((uintptr_t)animation + (uintptr_t)get32bit_LE((u8*)&raw_material_anim_group->color_lut));

			material_anims->frame_count = get32bit_LE((u8*)&raw_material_anim_group->frame_count);
			material_anims->current_frame = get16bit_LE((u8*)&raw_material_anim_group->anim_frame);
			material_anims->count = get32bit_LE((u8*)&raw_material_anim_group->anim_count);
			material_anims->animations = (CMaterialAnimation*) alloc_from_heap(material_anims->count * sizeof(CMaterialAnimation));

			for(j = 0; j < material_anims->count; j++) {
				MaterialAnimation* raw_anim = &raw_material_anims[j];
				CMaterialAnimation* animation = &material_anims->animations[j];

				memcpy(animation->material_name, raw_anim->material_name, 64);
				animation->diffuse_r_blend = raw_anim->diffuse_r_blend;
				animation->diffuse_g_blend = raw_anim->diffuse_g_blend;
				animation->diffuse_b_blend = raw_anim->diffuse_b_blend;
				animation->diffuse_r_lut_len = get16bit_LE((u8*)&raw_anim->diffuse_r_lut_len);
				animation->diffuse_g_lut_len = get16bit_LE((u8*)&raw_anim->diffuse_g_lut_len);
				animation->diffuse_b_lut_len = get16bit_LE((u8*)&raw_anim->diffuse_b_lut_len);
				animation->diffuse_r_lut_idx = get16bit_LE((u8*)&raw_anim->diffuse_r_lut_idx);
				animation->diffuse_g_lut_idx = get16bit_LE((u8*)&raw_anim->diffuse_g_lut_idx);
				animation->diffuse_b_lut_idx = get16bit_LE((u8*)&raw_anim->diffuse_b_lut_idx);
				animation->ambient_r_blend = raw_anim->ambient_r_blend;
				animation->ambient_g_blend = raw_anim->ambient_g_blend;
				animation->ambient_b_blend = raw_anim->ambient_b_blend;
				animation->ambient_r_lut_len = get16bit_LE((u8*)&raw_anim->ambient_r_lut_len);
				animation->ambient_g_lut_len = get16bit_LE((u8*)&raw_anim->ambient_g_lut_len);
				animation->ambient_b_lut_len = get16bit_LE((u8*)&raw_anim->ambient_b_lut_len);
				animation->ambient_r_lut_idx = get16bit_LE((u8*)&raw_anim->ambient_r_lut_idx);
				animation->ambient_g_lut_idx = get16bit_LE((u8*)&raw_anim->ambient_g_lut_idx);
				animation->ambient_b_lut_idx = get16bit_LE((u8*)&raw_anim->ambient_b_lut_idx);
				animation->specular_r_blend = raw_anim->specular_r_blend;
				animation->specular_g_blend = raw_anim->specular_g_blend;
				animation->specular_b_blend = raw_anim->specular_b_blend;
				animation->specular_r_lut_len = get16bit_LE((u8*)&raw_anim->specular_r_lut_len);
				animation->specular_g_lut_len = get16bit_LE((u8*)&raw_anim->specular_g_lut_len);
				animation->specular_b_lut_len = get16bit_LE((u8*)&raw_anim->specular_b_lut_len);
				animation->specular_r_lut_idx = get16bit_LE((u8*)&raw_anim->specular_r_lut_idx);
				animation->specular_g_lut_idx = get16bit_LE((u8*)&raw_anim->specular_g_lut_idx);
				animation->specular_b_lut_idx = get16bit_LE((u8*)&raw_anim->specular_b_lut_idx);
				animation->alpha_blend = raw_anim->alpha_blend;
				animation->alpha_lut_len = get16bit_LE((u8*)&raw_anim->alpha_lut_len);
				animation->alpha_lut_idx = get16bit_LE((u8*)&raw_anim->alpha_lut_idx);

				UPDATE_MAXFRAME(maxcolor, animation->diffuse_r_lut_idx + animation->diffuse_r_lut_len);
				UPDATE_MAXFRAME(maxcolor, animation->diffuse_g_lut_idx + animation->diffuse_g_lut_len);
				UPDATE_MAXFRAME(maxcolor, animation->diffuse_b_lut_idx + animation->diffuse_b_lut_len);
				UPDATE_MAXFRAME(maxcolor, animation->ambient_r_lut_idx + animation->ambient_r_lut_len);
				UPDATE_MAXFRAME(maxcolor, animation->ambient_g_lut_idx + animation->ambient_g_lut_len);
				UPDATE_MAXFRAME(maxcolor, animation->ambient_b_lut_idx + animation->ambient_b_lut_len);
				UPDATE_MAXFRAME(maxcolor, animation->specular_r_lut_idx + animation->specular_r_lut_len);
				UPDATE_MAXFRAME(maxcolor, animation->specular_g_lut_idx + animation->specular_g_lut_len);
				UPDATE_MAXFRAME(maxcolor, animation->specular_b_lut_idx + animation->specular_b_lut_len);
				UPDATE_MAXFRAME(maxcolor, animation->alpha_lut_idx + animation->alpha_lut_len);
			}

			printf("[%d] material animation group with %d frames\n", i, maxcolor);

			material_anims->color_lut = (u8*) alloc_from_heap(maxcolor);

			for(j = 0; j < maxcolor; j++)
				material_anims->color_lut[j] = color_lut[j];

			material_anims->time = 0;
		} else {
			anim->material_animations[i] = NULL;
		}

		if(node_animations[i]) {
			model->node_animations = (void*) 1;
		}
	}

	// select first animation
	for(i = 0; i < anim->count; i++) {
		if(texcoord_animations[i]) {
			parse_texcoord_animation(model, anim->texcoord_animations[i]);
			break;
		}
	}

	for(i = 0; i < anim->count; i++) {
		if(material_animations[i]) {
			parse_material_animation(model, anim->material_animations[i]);
			break;
		}
	}

	model->animation = anim;

	return anim;
}

void parse_texcoord_animation(CModel* model, CTexcoordAnimationGroup* animation_group)
{
	unsigned int i;
	int j;
	CMaterial* mtl;

	animation_group->current_frame = 0;
	model->texcoord_animations = animation_group;

	for(i = 0; i < model->num_materials; i++) {
		model->materials[i].texcoord_anim_id = -1;
		mtl = &model->materials[i];
		for(j = 0; j < animation_group->count; j++) {
			if(!strcmp(mtl->name, animation_group->animations[j].material_name)) {
				model->materials[i].texcoord_anim_id = j;
				// mtl = &model->materials[i];
				// if(mtl->texcoord_transform_mode == GX_TEXGEN_NONE)
				// 	mtl->texcoord_transform_mode = GX_TEXGEN_TEXCOORD;
				break;
			}
		}
	}
}

void parse_material_animation(CModel* model, CMaterialAnimationGroup* animation_group)
{
	unsigned int i;
	int j;
	CMaterial* mtl;

	animation_group->current_frame = 0;
	model->material_animations = animation_group;

	for(i = 0; i < model->num_materials; i++) {
		model->materials[i].material_anim_id = -1;
		mtl = &model->materials[i];
		for(j = 0; j < animation_group->count; j++) {
			if(!strcmp(mtl->name, animation_group->animations[j].material_name)) {
				model->materials[i].material_anim_id = j;
				break;
			}
		}
	}
}

float interpolate(float* values, int frame, int speed, int length, int frame_count)
{
	if(length == 1)
		return *values;

	if(speed == 1)
		return values[frame];

	// if we can't interpolate, use the last frame
	int limit = (frame_count - 1) >> (speed / 2) << (speed / 2);
	if(frame >= limit)
		return values[frame - limit + (frame >> (speed / 2))];

	// interpolate between two frames if necessary
	int idx_1 = frame >> (speed / 2);
	int idx_2 = (frame >> (speed / 2)) + 1;

	float div = 1 << (speed / 2);
	int t = frame & ((speed / 2) | 1);
	if(t) {
		return values[idx_1] * (1 - (float)t / div) + values[idx_2] * ((float)t / div);
	} else {
		return values[idx_1];
	}
}

float interpolate_rot(float* values, int frame, int speed, int length, int frame_count)
{
	if(length == 1)
		return *values;

	if(speed == 1)
		return values[frame];

	// if we can't interpolate, use the last frame
	int limit = (frame_count - 1) >> (speed / 2) << (speed / 2);
	if(frame >= limit)
		return values[frame - limit + (frame >> (speed / 2))];

	// interpolate between two frames if necessary
	int idx_1 = frame >> (speed / 2);
	int idx_2 = (frame >> (speed / 2)) + 1;

	float val_1 = values[idx_1];
	float val_2 = values[idx_2];

	if(val_1 - val_2 > M_PI) {
		val_2 += 2.0 * M_PI;
	} else if(val_1 - val_2 < -M_PI) {
		val_1 += 2.0 * M_PI;
	}

	float div = 1 << (speed / 2);
	int t = frame & ((speed / 2) | 1);
	if(t) {
		return val_1 * (1 - (float)t / div) + val_2 * ((float)t / div);
	} else {
		return val_1;
	}
}

u8 interpolate_color_channel(u8* values, int frame, int speed, int length, int frame_count)
{
	if(length == 1)
		return *values;

	if(speed == 1)
		return values[frame];

	// if we can't interpolate, use the last frame
	int limit = (frame_count - 1) >> (speed / 2) << (speed / 2);
	if(frame >= limit)
		return values[frame - limit + (frame >> (speed / 2))];

	// interpolate between two frames if necessary
	int idx_1 = frame >> (speed / 2);
	int idx_2 = (frame >> (speed / 2)) + 1;

	float div = 1 << (speed / 2);
	int t = frame & ((speed / 2) | 1);
	if(t) {
		return (u8) (values[idx_1] * (1.0 - (float)t / div) + values[idx_2] * ((float)t / div));
	} else {
		return values[idx_1];
	}
}

void process_texcoord_animation(CTexcoordAnimationGroup* group, int id, int width, int height)
{
	CTexcoordAnimation* anim = &group->animations[id];
	float scale_s = interpolate(&group->scales[anim->scale_s_idx], group->current_frame, anim->scale_s_blend, anim->scale_s_len, group->frame_count);
	float scale_t = interpolate(&group->scales[anim->scale_t_idx], group->current_frame, anim->scale_t_blend, anim->scale_t_len, group->frame_count);
	float rot = interpolate_rot(&group->rotations[anim->rot_idx], group->current_frame, anim->rot_blend, anim->rot_len, group->frame_count);
	float translate_s = interpolate(&group->translations[anim->translate_s_idx], group->current_frame, anim->translate_s_blend, anim->translate_s_len, group->frame_count);
	float translate_t = interpolate(&group->translations[anim->translate_t_idx], group->current_frame, anim->translate_t_blend, anim->translate_t_len, group->frame_count);

	glMatrixMode(GL_TEXTURE);
	glTranslatef(translate_s * width, translate_t * height, 0);
	if(rot != 0) {
		glTranslatef(width / 2, height / 2, 0);
		glRotatef(rot / M_PI * 180.0 , 0, 0, 1);
		glTranslatef(-width / 2, -height / 2, 0);
	}
	glScalef(scale_s, scale_t, 1);
}

void process_material_animation(CMaterialAnimationGroup* group, int id, CMaterial* material)
{
	CMaterialAnimation* anim = &group->animations[id];

	material->diffuse.r = interpolate_color_channel(&group->color_lut[anim->diffuse_r_lut_idx], group->current_frame, anim->diffuse_r_blend, anim->diffuse_r_lut_len, group->frame_count);
	material->diffuse.g = interpolate_color_channel(&group->color_lut[anim->diffuse_g_lut_idx], group->current_frame, anim->diffuse_g_blend, anim->diffuse_g_lut_len, group->frame_count);
	material->diffuse.b = interpolate_color_channel(&group->color_lut[anim->diffuse_b_lut_idx], group->current_frame, anim->diffuse_b_blend, anim->diffuse_b_lut_len, group->frame_count);

	material->ambient.r = interpolate_color_channel(&group->color_lut[anim->ambient_r_lut_idx], group->current_frame, anim->ambient_r_blend, anim->ambient_r_lut_len, group->frame_count);
	material->ambient.g = interpolate_color_channel(&group->color_lut[anim->ambient_g_lut_idx], group->current_frame, anim->ambient_g_blend, anim->ambient_g_lut_len, group->frame_count);
	material->ambient.b = interpolate_color_channel(&group->color_lut[anim->ambient_b_lut_idx], group->current_frame, anim->ambient_b_blend, anim->ambient_b_lut_len, group->frame_count);

	material->specular.r = interpolate_color_channel(&group->color_lut[anim->specular_r_lut_idx], group->current_frame, anim->specular_r_blend, anim->specular_r_lut_len, group->frame_count);
	material->specular.g = interpolate_color_channel(&group->color_lut[anim->specular_g_lut_idx], group->current_frame, anim->specular_g_blend, anim->specular_g_lut_len, group->frame_count);
	material->specular.b = interpolate_color_channel(&group->color_lut[anim->specular_b_lut_idx], group->current_frame, anim->specular_b_blend, anim->specular_b_lut_len, group->frame_count);

	if(!(material->anim_flags & 2))
		material->alpha = interpolate_color_channel(&group->color_lut[anim->alpha_lut_idx], group->current_frame, anim->alpha_blend, anim->alpha_lut_len, group->frame_count);
}

#define	FRAME_TIME	(1.0 / 30.0)

void CAnimation_process(CAnimation* animation, float dt)
{
	unsigned int i;
	for(i = 0; i < animation->count; i++) {
		if(animation->texcoord_animations[i]) {
			CTexcoordAnimationGroup* anim = animation->texcoord_animations[i];
			anim->time += dt;
			int inc = anim->time / FRAME_TIME;
			if(inc) {
				anim->current_frame += inc;
				anim->time -= inc * FRAME_TIME;
			}
			anim->current_frame %= anim->frame_count;
			// printf("[tex] frame %4d/%4d\n", anim->current_frame, anim->frame_count);
		}
		if(animation->material_animations[i]) {
			CMaterialAnimationGroup* anim = animation->material_animations[i];
			anim->time += dt;
			int inc = anim->time / FRAME_TIME;
			if(inc) {
				anim->current_frame += inc;
				anim->time -= inc * FRAME_TIME;
			}
			anim->current_frame %= anim->frame_count;
			// printf("[mtl] frame %4d/%4d\n", anim->current_frame, anim->frame_count);
		}
	}
}
