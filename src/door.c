#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "types.h"
#include "model.h"
#include "animation.h"
#include "texture_containers.h"
#include "entity.h"
#include "os.h"
#include "heap.h"

#define	NUM_DOORS 4

const char door_palette_ids[10] = { 0, 1, 2, 7, 6, 3, 4, 5, 0, 0 };

static const char* alimbic_door_locks[NUM_DOORS] = {
		"models/AlimbicDoorLock_mdl_Model.bin",
		"models/AlimbicMorphBallDoorLock_mdl_Model.bin",
		"models/AlimbicBossDoorLock_mdl_Model.bin",
		"models/ThinDoorLock_mdl_Model.bin"
};

static const char* door_lock_anim_names[NUM_DOORS] = {
		"models/AlimbicDoorLock_mdl_Anim.bin",
		"models/AlimbicMorphBallDoorLock_mdl_Anim.bin",
		"models/AlimbicBossDoorLock_mdl_Anim.bin",
		"models/ThinDoorLock_mdl_Anim.bin"
};

static const fx32 door_lock_offsets[4] = { 0x1666, 0xB33, 0x3800, 0x1666 };

static const char* alimbic_door_model_names[NUM_DOORS] = {
		"models/AlimbicDoor_Model.bin",
		"models/AlimbicMorphBallDoor_mdl_Model.bin",
		"models/AlimbicBossDoor_Model.bin",
		"models/AlimbicThinDoor_Model.bin"
};

static const char *alimbic_door_animation_names[NUM_DOORS] = {
		"models/AlimbicDoor_Anim.bin",
		"models/AlimbicMorphBallDoor_mdl_Anim.bin",
		"models/AlimbicBossDoor_Anim.bin",
		"models/AlimbicThinDoor_Anim.bin"
};

static const u16 door_scan_ids[10] = { 0, 0xFF, 0x108, 0xFC, 0x100, 0xFD, 0xFE, 0xF9, 0x10A, 0x109 };
static const u16 door_default_scan_id = 0xFB;

static CModel* alimbic_door_models[NUM_DOORS] = { 0 };
static CAnimation* alimbic_door_animations[NUM_DOORS] = { 0 };
static CModel* alimbic_door_lock_models[NUM_DOORS] = { 0 };
static CAnimation* alimbic_door_lock_animations[NUM_DOORS] = { 0 };
CModel* alimbic_palettes_model = NULL;

void load_alimbic_doors(int type)
{
	const char* model_name;
	const char* door_animation;
	CModel* door_lock_model;

	if(!alimbic_door_models[type]) {
		model_name = alimbic_door_model_names[type];
		if(model_name) {
			load_model(&alimbic_door_models[type], model_name, 0);
			if(type == 1) {
				if(alimbic_door_models[1]->textures || alimbic_door_models[1]->palettes)
					OS_Terminate();
				load_texture_container(alimbic_door_models[1], ALIMBIC_TXTR);
			}
			// alimbic_door_models[type]->flags |= 1u;
		}
		door_animation = alimbic_door_animation_names[type];
		if(door_animation)
			load_animation(&alimbic_door_animations[type], door_animation, alimbic_door_models[type], 0);
	}

	if(!alimbic_door_lock_models[type]) {
		load_model(&alimbic_door_lock_models[type], alimbic_door_locks[type], 0);
		door_lock_model = alimbic_door_lock_models[type];
		if(door_lock_model->textures || door_lock_model->palettes)
			OS_Terminate();
		load_texture_container(alimbic_door_lock_models[type], ALIMBIC_TXTR);
		// alimbic_door_lock_models[type]->flags |= 1u;
		load_animation(&alimbic_door_lock_animations[type], door_lock_anim_names[type], alimbic_door_lock_models[type], 0);
	}

	if(!alimbic_palettes_model)
		load_model(&alimbic_palettes_model, "models/AlimbicPalettes_pal_Model.bin", 0);
}

CEntity* CAlimbicDoor_construct(const char* node_name, EntityData* data)
{
	if(data->type != ALIMBIC_DOOR)
		OS_Terminate();

	EntityDoor* door = (EntityDoor*)data;

	printf("Door: type=%d [pal=%d] target_layer=0x%02x target_room=\"%s\" node=\"%s\"\n", door->type, door->palette_id, door->target_layer_id, door->room_name, door->node_name);

	CAlimbicDoor* obj = (CAlimbicDoor*)alloc_from_heap(sizeof(CAlimbicDoor));
	CEntityCtor(&obj->base, data);

	obj->ent = door;

	obj->pos.x = FX_FX32_TO_F32(door->pos.x);
	obj->pos.y = FX_FX32_TO_F32(door->pos.y);
	obj->pos.z = FX_FX32_TO_F32(door->pos.z);

	get_transform_mtx(&obj->transform, &door->vec2, &door->vec1);
	obj->transform.m[3][0] = obj->pos.x;
	obj->transform.m[3][1] = obj->pos.y;
	obj->transform.m[3][2] = obj->pos.z;

	int type = door->type;
	obj->type = type;
	obj->palette_id = door->palette_id;

	obj->flags = 2; // locked

	load_alimbic_doors(type);
	obj->model = alimbic_door_models[type]; // default: no modification

	if(!type || type == 3) {
		obj->model = (CModel*) alloc_from_heap(sizeof(CModel));
		memcpy(obj->model, alimbic_door_models[type], sizeof(CModel));
		obj->model->materials = (MATERIAL*) alloc_from_heap(sizeof(MATERIAL) * obj->model->num_materials);
		memcpy(obj->model->materials, alimbic_door_models[type]->materials, sizeof(MATERIAL) * obj->model->num_materials);
		obj->model->palettes = (PALETTE*) alloc_from_heap(sizeof(PALETTE) * obj->model->num_palettes);
		memcpy(obj->model->palettes, alimbic_door_models[type]->palettes, sizeof(PALETTE) * obj->model->num_palettes);

		int palid;
		if(obj->flags & 2)
			palid = door_palette_ids[obj->palette_id];
		else
			palid = door_palette_ids[9];

		memcpy(&obj->model->palettes[1], &alimbic_palettes_model->palettes[palid], sizeof(PALETTE));

		if(type == 3) {
			memcpy(&obj->model->palettes[2], &alimbic_palettes_model->palettes[palid], sizeof(PALETTE));
		}

		CModel_set_textures(obj->model);
	}

	return (CEntity*)obj;
}

void CAlimbicDoor_process_class(float dt)
{
	unsigned int i;
	for(i = 0; i < NUM_DOORS; i++) {
		if(alimbic_door_animations[i])
			CAnimation_process(alimbic_door_animations[i], dt);
		if(alimbic_door_lock_animations[i])
			CAnimation_process(alimbic_door_lock_animations[i], dt);
	}
}

void CAlimbicDoor_render(CEntity* obj)
{
	CAlimbicDoor* self = (CAlimbicDoor*)obj;

	if(!self->model)
		return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(self->transform.a);

	CModel_render_all(self->model);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

Vec3* CAlimbicDoor_get_position(CEntity* obj)
{
	CAlimbicDoor* self = (CAlimbicDoor*)obj;

	return &self->pos;
}

void CAlimbicDoor_set_tex_filter(int type)
{
	unsigned int i;
	for(i = 0; i < NUM_DOORS; i++) {
		if(alimbic_door_models[i])
			CModel_set_texture_filter(alimbic_door_models[i], type);
		if(alimbic_door_lock_models[i])
			CModel_set_texture_filter(alimbic_door_lock_models[i], type);
	}
}

void EntAlimbicDoorRegister(void)
{
	EntityClass* ent = EntRegister(ALIMBIC_DOOR);
	ent->construct = CAlimbicDoor_construct;
	ent->process_class = CAlimbicDoor_process_class;
	ent->render = CAlimbicDoor_render;
	ent->get_position = CAlimbicDoor_get_position;
	ent->set_tex_filter = CAlimbicDoor_set_tex_filter;
}
