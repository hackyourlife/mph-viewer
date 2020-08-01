#include <stdio.h>
#include <string.h>
#include <GL/gl.h>

#include "types.h"
#include "model.h"
#include "animation.h"
#include "texture_containers.h"
#include "entity.h"
#include "vec.h"
#include "os.h"
#include "heap.h"

static const u16 force_field_scan_ids[10] = { 0, 0x126, 0x127, 0x123, 0x122, 0x124, 0x125, 0x128, 0, 0x10B };

static CModel* force_field_model = NULL;
static CAnimation* force_field_anim = NULL;
static CModel* force_field_lock_model = NULL;
static CAnimation* force_field_lock_anim = NULL;

extern const char door_palette_ids[10];
extern CModel* alimbic_palettes_model;

CEntity* CForceField_construct(const char* node_name, EntityData* data)
{
	if(data->type != FORCE_FIELD)
		OS_Terminate();

	EntityForceField* force_field = (EntityForceField*)data;

	printf("ForceField: type=%d\n", force_field->type);

	CForceField* obj = (CForceField*)alloc_from_heap(sizeof(CForceField));
	CEntityCtor(&obj->base, data);

	obj->ent = force_field;
	obj->type = force_field->type;

	obj->flags = 1; // activate

	if(obj->flags & 1) {
		obj->base.scan_id = force_field_scan_ids[force_field->type];
		obj->alpha = 1;
	} else {
		obj->base.scan_id = 0;
		obj->alpha = 0;
	}

	obj->pos.x = FX_FX32_TO_F32(force_field->pos.x);
	obj->pos.y = FX_FX32_TO_F32(force_field->pos.y);
	obj->pos.z = FX_FX32_TO_F32(force_field->pos.z);

	get_transform_mtx(&obj->transform, &force_field->vec2, &force_field->vec1);
	obj->transform.m[3][0] = obj->pos.x;
	obj->transform.m[3][1] = obj->pos.y;
	obj->transform.m[3][2] = obj->pos.z;

	obj->transform.m[0][0] *= FX_FX32_TO_F32(force_field->width);
	obj->transform.m[0][1] *= FX_FX32_TO_F32(force_field->width);
	obj->transform.m[0][2] *= FX_FX32_TO_F32(force_field->width);
	obj->transform.m[1][0] *= FX_FX32_TO_F32(force_field->height);
	obj->transform.m[1][1] *= FX_FX32_TO_F32(force_field->height);
	obj->transform.m[1][2] *= FX_FX32_TO_F32(force_field->height);

	VecFx32 normal;

	VEC_CrossProduct(&force_field->vec1, &force_field->vec2, &normal);
	VEC_Normalize(&normal, &normal);

	obj->model = (CModel*) alloc_from_heap(sizeof(CModel));
	memcpy(obj->model, force_field_model, sizeof(CModel));
	obj->model->materials = (CMaterial*) alloc_from_heap(sizeof(CMaterial) * obj->model->num_materials);
	memcpy(obj->model->materials, force_field_model->materials, sizeof(CMaterial) * obj->model->num_materials);
	obj->model->palettes = (CPalette*) alloc_from_heap(sizeof(CPalette) * obj->model->num_palettes);
	memcpy(obj->model->palettes, force_field_model->palettes, sizeof(CPalette) * obj->model->num_palettes);

	int palid = door_palette_ids[obj->type];
	memcpy(obj->model->palettes, &alimbic_palettes_model->palettes[palid], sizeof(CPalette));

	CModel_set_textures(obj->model);

	// TODO: if(obj->flags & 1 && force_field->type != 9) spawn(ENEMY_INSTANCE, 49);

	return (CEntity*)obj;
}

void CForceField_process_class(float dt)
{
	CAnimation_process(force_field_anim, dt);
}

void CForceField_process(CEntity* obj, float dt)
{
	CForceField* self = (CForceField*)obj;

	if(self->flags & 1) {
		if(self->alpha < 1) {
			self->alpha += dt;
			if(self->alpha > 1)
				self->alpha = 1;
		}
	} else if(self->alpha) {
		self->alpha -= dt;
		if(self->alpha < 0)
			self->alpha = 0;
	}
}

void CForceField_set_state(CEntity* obj, int state)
{
	CForceField* self = (CForceField*)obj;

	if(state)
		self->flags |= 1;
	else
		self->flags &= ~1;
}

void CForceField_render(CEntity* obj)
{
	CForceField* self = (CForceField*)obj;

	if(self->alpha == 0)
		return;

	CModel_render_all(self->model, &self->transform, self->alpha);
}

Vec3* CForceField_get_position(CEntity* obj)
{
	CForceField* self = (CForceField*)obj;

	return &self->pos;
}

void CForceField_set_tex_filter(int type)
{
	CModel_set_texture_filter(force_field_model, type);
}

void EntForceFieldRegister(void)
{
	EntityClass* ent = EntRegister(FORCE_FIELD);
	ent->construct = CForceField_construct;
	ent->process_class = CForceField_process_class;
	ent->process = CForceField_process;
	ent->render = CForceField_render;
	ent->get_position = CForceField_get_position;
	ent->set_tex_filter = CForceField_set_tex_filter;

	if(!force_field_model) {
		load_model(&force_field_model, "models/ForceField_Model.bin", 0);
		load_animation(&force_field_anim, "models/ForceField_Anim.bin", force_field_model, 0);
		load_model(&force_field_lock_model, "models/ForceFieldLock_mdl_Model.bin", 0);
		load_animation(&force_field_lock_anim, "models/ForceFieldLock_mdl_Anim.bin", force_field_lock_model, 0);
		if(force_field_lock_model->textures || force_field_lock_model->palettes)
			OS_Terminate();
		load_texture_container(force_field_lock_model, 0);
	}

	if(!alimbic_palettes_model)
		load_model(&alimbic_palettes_model, "models/AlimbicPalettes_pal_Model.bin", 0);
}
