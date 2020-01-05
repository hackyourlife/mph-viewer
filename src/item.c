#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>

#include "types.h"
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

static const char pickup_has_anim[22] = {
	0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0
};

static CModel* pickup_models[22] = { 0 };
static CAnimation* pickup_animations[22] = { 0 };

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

////////////////////////////////////////////////////////////////////////////////
// normal items
////////////////////////////////////////////////////////////////////////////////
static void CItem_dump(CItem* item)
{
	printf("CItem [%s]\n", pickup_model_names[item->model_id]);
	printf("field_0C=0x%x [%f]\n", item->item->field_C,  FX_FX32_TO_F32(item->item->field_C));
	printf("field_10=0x%x [%f]\n", item->item->field_10, FX_FX32_TO_F32(item->item->field_10));
	printf("field_14=0x%x [%f]\n", item->item->field_14, FX_FX32_TO_F32(item->item->field_14));
	printf("field_18=0x%x [%f]\n", item->item->field_18, FX_FX32_TO_F32(item->item->field_18));
	printf("field_1C=0x%x [%f]\n", item->item->field_1C, FX_FX32_TO_F32(item->item->field_1C));
	printf("field_20=0x%x [%f]\n", item->item->field_20, FX_FX32_TO_F32(item->item->field_20));
	printf("item_id=0x%x\n", item->item->item_id);
	printf("id=0x%x\n", item->item->id);
	printf("field_2C=0x%x [%f]\n", item->item->field_2C, FX_FX32_TO_F32(item->item->field_2C));
	printf("field_30=0x%x [%f]\n", item->item->field_30, FX_FX32_TO_F32(item->item->field_30));
	printf("field_34=0x%x [%f]\n", item->item->field_34, FX_FX32_TO_F32(item->item->field_34));
	printf("field_38=0x%x [%f]\n", item->item->field_38, FX_FX32_TO_F32(item->item->field_38));
	printf("field_3C=0x%x [%f]\n", item->item->field_3C, FX_FX32_TO_F32(item->item->field_3C));
	printf("field_40=0x%x [%f]\n", item->item->field_40, FX_FX32_TO_F32(item->item->field_40));
	printf("field_44=0x%x [%f]\n", item->item->field_44, FX_FX32_TO_F32(item->item->field_44));
	printf("\n");
}

CEntity* CItem_construct(const char* node_name, EntityData* data)
{
	if(data->type != ITEM) {
		OS_Terminate();
	}

	EntityItem* item = (EntityItem*)data;

	printf("Item: %d [%s]\n", data->type, pickup_model_names[item->id]);

	CItem* obj = (CItem*)alloc_from_heap(sizeof(CItem));
	CEntityCtor(&obj->base, data);

	load_pickup(item->id);
	obj->model_id = item->id;

	obj->pos.x = FX_FX32_TO_F32(item->pos.x);
	obj->pos.y = FX_FX32_TO_F32(item->pos.y);
	obj->pos.z = FX_FX32_TO_F32(item->pos.z);

	obj->item = item;
	obj->rotation = (float)rand() / (float)RAND_MAX * 360.0;
	printf("initial rotation: %f\n", obj->rotation);

	CItem_dump(obj);
	return (CEntity*)obj;
}

void CItem_process(CEntity* obj, float dt)
{
	CItem* self = (CItem*)obj;

	self->rotation += dt * 360.0 * 0.35;
	while(self->rotation > 360.0)
		self->rotation -= 360.0;
}

void CItem_render(CEntity* obj)
{
	CItem* self = (CItem*)obj;

	float off = -pickup_models[self->model_id]->min_y + (sinf(self->rotation / 180.0 * M_PI) + 1.0) / 8.0f;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

#if 0
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
	glColor4f(1, 1, 1, 1);
#endif

	glTranslatef(self->pos.x, self->pos.y + off, self->pos.z);

	glRotatef(self->rotation, 0, 1, 0);

	CModel_render(pickup_models[self->model_id]);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

Vec3* CItem_get_position(CEntity* obj)
{
	CItem* self = (CItem*)obj;

	return &self->pos;
}

void EntItemRegister(void)
{
	EntityClass* ent = EntRegister(ITEM);
	ent->construct = CItem_construct;
	ent->process = CItem_process;
	ent->render = CItem_render;
	ent->get_position = CItem_get_position;
}

////////////////////////////////////////////////////////////////////////////////
// delayed items
////////////////////////////////////////////////////////////////////////////////
CEntity* CDelayedItem_construct(const char* node_name, EntityData* data)
{
	if(data->type != PICKUP) {
		OS_Terminate();
	}

	EntityItem* item = (EntityItem*)data;

	printf("Delayed item: %d [%s]\n", data->type, NULL); // pickup_model_names[item->id]);

	CItem* obj = (CItem*)alloc_from_heap(sizeof(CItem));
	CEntityCtor(&obj->base, data);

	//load_pickup(item->id);
	//obj->model_id = item->id;

	obj->pos.x = FX_FX32_TO_F32(item->pos.x);
	obj->pos.y = FX_FX32_TO_F32(item->pos.y);
	obj->pos.z = FX_FX32_TO_F32(item->pos.z);

	return (CEntity*)obj;
}

void CDelayedItem_render(CEntity* obj)
{
	CItem* self = (CItem*)obj;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(self->pos.x, self->pos.y, self->pos.z);

	glRotatef(360.0f - yrot, 0, -1, 0);
	glRotatef(xrot, -1, 0, 0);

	// CModel_render(pickup_models[self->model_id]);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void EntDelayedItemRegister(void)
{
	EntityClass* ent = EntRegister(PICKUP);
	ent->construct = CDelayedItem_construct;
	ent->render = CDelayedItem_render;
}
