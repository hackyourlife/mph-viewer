#include <stdio.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "types.h"
#include "vec.h"
#include "mtx.h"
#include "model.h"
#include "animation.h"
#include "entity.h"
#include "os.h"
#include "heap.h"

#ifdef WIN32
extern PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf;
extern PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixf;
#endif

#define	NUM_JUMP_PADS	6

static const char* jump_pads[NUM_JUMP_PADS] = {
	"JumpPad",
	"JumpPad_Alimbic",
	"JumpPad_Ice",
	"JumpPad_IceStation",
	"JumpPad_Lava",
	"JumpPad_LavaStation"
};

static CModel* jump_pad_models[NUM_JUMP_PADS] = { 0 };
static CAnimation* jump_pad_anims[NUM_JUMP_PADS] = { 0 };
static CModel* jump_pad_beam_model = NULL;
static CAnimation* jump_pad_beam_animation = NULL;

static CModel* jumppad_load_model(EntityJumpPad* jump_pad)
{
	char filename[256];

	int model_id = jump_pad->model_id;
	if(!jump_pad_models[model_id] && jump_pads[model_id]) {
		sprintf(filename, "models/%s_Model.bin", jump_pads[model_id]);
		load_model(&jump_pad_models[model_id], filename, 0);
		sprintf(filename, "models/%s_Anim.bin", jump_pads[model_id]);
		load_animation(&jump_pad_anims[model_id], filename, jump_pad_models[model_id], 0);
	}

	if(!jump_pad_beam_model) {
		load_model(&jump_pad_beam_model, "models/JumpPad_Beam_Model.bin", 0);
		load_animation(&jump_pad_beam_animation, "models/JumpPad_Beam_Anim.bin", jump_pad_beam_model, 0);
	}
}

static void CJumpPad_set_beam_mtx(CJumpPad* self, Mtx44* base)
{
	VecFx32 beam_vec;
	VecFx32 up = VECFX32(0, 1, 0);
	VecFx32 right = VECFX32(1, 0, 0);
	VEC_Normalize(&self->pad->beam_vec, &beam_vec);
	MTX44MultVec33(&beam_vec, base, &self->beam_vec);

	beam_vec.x = FX_F32_TO_FX32(self->beam_vec.x);
	beam_vec.y = FX_F32_TO_FX32(self->beam_vec.y);
	beam_vec.z = FX_F32_TO_FX32(self->beam_vec.z);
	if(beam_vec.x || beam_vec.z) {
		get_transform_mtx(&self->beam_mtx, &beam_vec, &up);
	} else {
		get_transform_mtx(&self->beam_mtx, &beam_vec, &right);
	}
	self->beam_mtx.m[3][0] = self->pos.x;
	self->beam_mtx.m[3][1] = self->pos.y + 0.25;
	self->beam_mtx.m[3][2] = self->pos.z;
	float speed = self->pad->speed;
	self->beam_vec.x *= speed;
	self->beam_vec.y *= speed;
	self->beam_vec.z *= speed;
}

void CJumpPad_process_class(float dt)
{
	for(int i = 0; i < NUM_JUMP_PADS; i++) {
		if(jump_pad_anims[i])
			CAnimation_process(jump_pad_anims[i], dt);
	}
	if(jump_pad_beam_animation)
		CAnimation_process(jump_pad_beam_animation, dt);
}

CEntity* CJumpPad_construct(const char* node_name, EntityData* data)
{
	VecFx32 up = VECFX32(0, 1, 0);

	if(data->type != JUMP_PAD) {
		OS_Terminate();
	}

	EntityJumpPad* jump_pad = (EntityJumpPad*)data;

	CJumpPad* obj = (CJumpPad*)alloc_from_heap(sizeof(CJumpPad));
	CEntityCtor(&obj->base, data);

	jumppad_load_model(jump_pad);
	obj->model_id = jump_pad->model_id;
	obj->pad = jump_pad;

	obj->pos.x = FX_FX32_TO_F32(jump_pad->pos.x);
	obj->pos.y = FX_FX32_TO_F32(jump_pad->pos.y);
	obj->pos.z = FX_FX32_TO_F32(jump_pad->pos.z);

	get_transform_mtx(&obj->base_mtx, &jump_pad->base_vec2, &jump_pad->base_vec1);
	obj->base_mtx.m[3][0] = obj->pos.x;
	obj->base_mtx.m[3][1] = obj->pos.y;
	obj->base_mtx.m[3][2] = obj->pos.z;

	CJumpPad_set_beam_mtx(obj, &obj->base_mtx);

	return (CEntity*)obj;
}

void CJumpPad_render(CEntity* obj)
{
	CJumpPad* self = (CJumpPad*)obj;

	CModel_render_all(jump_pad_models[self->model_id], &self->base_mtx, 1.0);
	CModel_render_all(jump_pad_beam_model, &self->beam_mtx, 1.0);
}

Vec3* CJumpPad_get_position(CEntity* obj)
{
	CJumpPad* self = (CJumpPad*)obj;

	return &self->pos;
}

void CJumpPad_set_tex_filter(int type)
{
	for(int i = 0; i < NUM_JUMP_PADS; i++) {
		if(jump_pad_models[i])
			CModel_set_texture_filter(jump_pad_models[i], type);
	}
	if(jump_pad_beam_model)
		CModel_set_texture_filter(jump_pad_beam_model, type);
}

void EntJumpPadRegister(void)
{
	EntityClass* ent = EntRegister(JUMP_PAD);
	ent->construct = CJumpPad_construct;
	ent->process_class = CJumpPad_process_class;
	ent->render = CJumpPad_render;
	ent->get_position = CJumpPad_get_position;
	ent->set_tex_filter = CJumpPad_set_tex_filter;
}
