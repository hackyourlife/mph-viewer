#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>

#include "types.h"
#include "mtx.h"
#include "model.h"
#include "animation.h"
#include "entity.h"
#include "os.h"
#include "heap.h"

extern float xrot, yrot;

#define NUM_PICKUP_MODELS 22

static const char* pickup_model_names[NUM_PICKUP_MODELS] = {
		"models/pick_health_B",
		"models/pick_health_A",
		"models/pick_health_C",
		"models/pick_dblDamage",
		"models/PickUp_EnergyExp",
		"models/pick_wpn_electro",
		"models/PickUp_MissileExp",
		"models/pick_wpn_jackhammer",
		"models/pick_wpn_snipergun",
		"models/pick_wpn_shotgun",
		"models/pick_wpn_mortar",
		"models/pick_wpn_ghostbuster",
		"models/pick_wpn_gorea",
		"models/pick_ammo_green",
		"models/pick_ammo_green",
		"models/pick_ammo_orange",
		"models/pick_ammo_orange",
		"models/pick_invis",
		"models/PickUp_AmmoExp",
		"models/Artifact_Key",
		"models/pick_deathball",
		"models/pick_wpn_all"
};

static const char pickup_has_anim[NUM_PICKUP_MODELS] = {
	0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0
};

static const u16 item_scan_ids[NUM_PICKUP_MODELS] = {
	9, 0xB, 0xC, 0, 0xA, 0x15, 0xD, 0x17, 0x16, 0x12, 0x14, 0x13, 0x1C, 0xE, 0xF, 0x10, 0x11, 0, 0x18, 0x1CF, 0, 0
};

static CModel* pickup_models[NUM_PICKUP_MODELS] = { 0 };
static CAnimation* pickup_animations[NUM_PICKUP_MODELS] = { 0 };

static CModel* item_base_model = NULL;

static void load_pickup(int id)
{
	char filename[256];

	if(id >= NUM_PICKUP_MODELS) {
		printf("invalid pickup model id: %d\n", id);
		return;
	}

	if(!pickup_models[id]) {
		const char* model_name = pickup_model_names[id];
		sprintf(filename, "%s_Model.bin", model_name);
		load_model(&pickup_models[id], filename, 0);
		if(pickup_has_anim[id]) {
			sprintf(filename, "%s_Anim.bin", pickup_model_names[id]);
			load_animation(&pickup_animations[id], filename, pickup_models[id], 0);
		}
	}
}

CEntity* CItem_construct(const char* node_name, EntityData* data)
{
	if(data->type != ITEM) {
		OS_Terminate();
	}

	EntityItem* item = (EntityItem*)data;

	printf("Item: %d [%s], spawn delay: %d, max spawn count: %d\n", data->type, pickup_model_names[item->type], item->spawn_delay, item->max_spawn_count);

	CItem* obj = (CItem*)alloc_from_heap(sizeof(CItem));
	CEntityCtor(&obj->base, data);

	load_pickup(item->type);
	obj->model_id = item->type;
	obj->base.scan_id = item_scan_ids[item->type];

	obj->pos.x = FX_FX32_TO_F32(item->pos.x);
	obj->pos.y = FX_FX32_TO_F32(item->pos.y);
	obj->pos.z = FX_FX32_TO_F32(item->pos.z);

	obj->enabled = item->enabled;
	obj->has_base = item->has_base;
	obj->max_spawn_count = item->max_spawn_count;
	obj->spawn_interval = item->spawn_interval;
	obj->spawn_delay = item->spawn_delay;

	obj->item = item;
	obj->rotation = (float)rand() / (float)RAND_MAX * 360.0;
	printf("initial rotation: %f\n", obj->rotation);

	return (CEntity*)obj;
}

void CItem_process_class(float dt)
{
	for(int i = 0; i < NUM_PICKUP_MODELS; i++) {
		if(pickup_animations[i])
			CAnimation_process(pickup_animations[i], dt);
	}
}

void CItem_process(CEntity* obj, float dt)
{
	CItem* self = (CItem*)obj;

	self->rotation += dt * 360.0 * 0.35;
	while(self->rotation > 360.0)
		self->rotation -= 360.0;
}

float CItem_get_y(fx32 y)
{
	// if y <= -2663 then y += 2663
	// else if y == 6393 then y += 2812
	// else y += 2662
	if(y <= -2663)
		return FX_FX32_TO_F32(y + 2663);
	else if(y == 6393)
		return FX_FX32_TO_F32(y + 2812);
	else
		return FX_FX32_TO_F32(y + 2662);
}

void CItem_render(CEntity* obj)
{
	CItem* self = (CItem*)obj;
	Mtx44 mtx;

	float y = CItem_get_y(self->item->pos.y) + (sinf(self->rotation / 180.0 * M_PI) + 1.0) / 8.0f;

	if(self->has_base) {
		MTX44Trans(&mtx, self->pos.x, self->pos.y, self->pos.z);
		CModel_render_all(item_base_model, &mtx, 1.0);
	}

	if(self->enabled) {
		Mtx44 rot;
		MTX44Trans(&mtx, self->pos.x, y, self->pos.z);
		MTX44RotRad(&rot, 'y', self->rotation / 180.0 * M_PI);
		MTX44Concat(&mtx, &rot, &mtx);

		CModel_render_all(pickup_models[self->model_id], &mtx, 1.0);
	}
}

Vec3* CItem_get_position(CEntity* obj)
{
	CItem* self = (CItem*)obj;

	return &self->pos;
}

void CItem_set_tex_filter(int type)
{
	unsigned int i;
	for(i = 0; i < 22; i++) {
		if(pickup_models[i]) {
			CModel_set_texture_filter(pickup_models[i], type);
		}
	}
}

void EntItemRegister(void)
{
	EntityClass* ent = EntRegister(ITEM);
	ent->construct = CItem_construct;
	ent->process = CItem_process;
	ent->process_class = CItem_process_class;
	ent->render = CItem_render;
	ent->get_position = CItem_get_position;
	// ent->set_tex_filter = CItem_set_tex_filter;

	load_model(&item_base_model, "common/items_base_Model.bin", USE_ARCHIVE);
}
