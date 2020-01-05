#include <stdio.h>
#include <GL/gl.h>

#include "types.h"
#include "model.h"
#include "animation.h"
#include "entity.h"
#include "os.h"
#include "heap.h"

static const char* jump_pads[6] = {
	"JumpPad",
	"JumpPad_Alimbic",
	"JumpPad_Ice",
	"JumpPad_IceStation",
	"JumpPad_Lava",
	"JumpPad_LavaStation"
};

static CModel* jump_pad_models[6] = { 0 };
static CAnimation* jump_pad_anims[6] = { 0 };
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

CEntity* CJumpPad_construct(const char* node_name, EntityData* data)
{
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

	return (CEntity*)obj;
}

void CJumpPad_render(CEntity* obj)
{
	CJumpPad* self = (CJumpPad*)obj;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(self->pos.x, self->pos.y, self->pos.z);

	CModel_render(jump_pad_models[self->model_id]);

	glMatrixMode(GL_MODELVIEW);
	glTranslatef(0, 0.2, 0);
	glRotatef(-90, 1, 0, 0);
	CModel_render(jump_pad_beam_model);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

#if 0
	glPointSize(10);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	glColor4f(1, 0, 0, 1);
	glVertex3f(self->pos.x - 0.5, self->pos.y, self->pos.z - 0.5);
	glVertex3f(self->pos.x - 0.5, self->pos.y, self->pos.z + 0.5);
	glVertex3f(self->pos.x + 0.5, self->pos.y, self->pos.z + 0.5);
	glVertex3f(self->pos.x + 0.5, self->pos.y, self->pos.z - 0.5);
	glEnd();
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
#endif
}

Vec3* CJumpPad_get_position(CEntity* obj)
{
	CJumpPad* self = (CJumpPad*)obj;

	return &self->pos;
}

void EntJumpPadRegister(void)
{
	EntityClass* ent = EntRegister(JUMP_PAD);
	ent->construct = CJumpPad_construct;
	ent->render = CJumpPad_render;
	ent->get_position = CJumpPad_get_position;
}
