#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "endianess.h"
#include "types.h"
#include "vec.h"
#include "error.h"
#include "fs.h"
#include "heap.h"
#include "model.h"
#include "entity.h"

void EntLoad(Entity** ent, const char* filename, int layer_id)
{
	unsigned int i;
	FSFile file;
	EntityFileHeader header;

	FS_InitFile(&file);
	if(!FS_OpenFile(&file, filename))
		fatal_error("EntLoad: Cannot open file %s!\n", filename);
	if(FS_ReadFile(&file, &header, sizeof(EntityFileHeader)) != sizeof(EntityFileHeader))
		fatal_error("EntLoad: Failed to read the file %s\n", filename);
	if(header.version != 2)
		fatal_error("EntLoad(): Error! Entity file \"%s\" is the wrong version!\n", filename);

	unsigned int length = header.lengths[layer_id];
	*ent = (Entity*) alloc_from_heap((length + 1) * sizeof(Entity));

	assert(sizeof(EntityFileHeader) == 36);
	assert(sizeof(EntityFileEntry) == 24);

	for(i = 0; i < length;) {
		Entity* entity = &(*ent)[i];
		EntityFileEntry entry;
		if(FS_ReadFile(&file, &entry, sizeof(EntityFileEntry)) != sizeof(EntityFileEntry))
			fatal_error("EntLoad: Failed to read the file %s!\n", filename);
		entity->layer_mask = get16bit_LE((u8*)&entry.layer_mask);
		entity->length = get32bit_LE((u8*)&entry.length);
		entity->data = NULL;
		if(entity->layer_mask & (1 << layer_id)) {
			int offset = FS_GetPosition(&file);
			FS_SeekFile(&file, get32bit_LE((u8*)&entry.data), FS_SEEK_SET);
			entity->data = (EntityData*) alloc_from_heap(entity->length);
			if(FS_ReadFile(&file, entity->data, entity->length) != entity->length)
				fatal_error("EntLoad: Failed to read the file %s!\n", filename);
			FS_SeekFile(&file, offset, FS_SEEK_SET);

			entity->data->type = get16bit_LE((u8*)&entity->data->type);
			strncpy(entity->node_name, entry.node_name, sizeof(entity->node_name));
			printf("entity %d: type=0x%x [length=%d, name='%s', mask=0x%x]\n", i, entity->data->type, entity->length, entity->node_name, entity->layer_mask);
			i++;
		} else {
			u16 type;
			int offset = FS_GetPosition(&file);
			FS_SeekFile(&file, get32bit_LE((u8*)&entry.data), FS_SEEK_SET);
			if(FS_ReadFile(&file, &type, 2) != 2)
				fatal_error("EntLoad: Failed to read the file %s!\n", filename);
			FS_SeekFile(&file, offset, FS_SEEK_SET);
			printf("entity %d: type=0x%x [ignored, mask=0x%x]\n", i, type, entity->layer_mask);
		}
	}

	Entity* end = &(*ent)[length];
	memset(end, 0, sizeof(Entity));

	FS_CloseFile(&file);
}

static EntityClass* entity_registry;
static CEntity** instances;
static int class_count;

void EntInitialize(int size)
{
	class_count = size;

	entity_registry = alloc_from_heap(size * sizeof(EntityClass));
	instances = alloc_from_heap(size * sizeof(CEntity*));

	memset(entity_registry, 0, size * sizeof(EntityClass));
	memset(instances, 0, size * sizeof(CEntity*));

	EntJumpPadRegister();
	EntItemRegister();
	EntObjectRegister();
	EntTeleporterRegister();
	EntAlimbicDoorRegister();
	EntPlatformRegister();
}

void CEntityCtor(CEntity* entity, EntityData* data)
{
	entity->type = data->type;
	entity->funcs = &entity_registry[data->type];
	entity->next = NULL;
}

static CEntity* default_ctor(const char* node_name, EntityData* entity)
{
	return NULL;
}

static void default_process(CEntity* self, float dt)
{
	// nothing
}

static void default_process_class(float dt)
{
	// nothing
}

static void default_render(CEntity* self)
{
	// nothing
}

static void default_set_tex_filter(int type)
{
	// nothing
}

static Vec3 pos0 = { 0, 0, 0 };
static Vec3* default_get_position(CEntity* self)
{
	return &pos0;
}

EntityClass* EntRegister(int id)
{
	EntityClass* ent = &entity_registry[id];
	ent->construct = default_ctor;
	ent->process = default_process;
	ent->process_class = default_process_class;
	ent->render = default_render;
	ent->get_position = default_get_position;
	ent->set_tex_filter = default_set_tex_filter;
	return ent;
}

static void CEntity_add(CEntity* entity)
{
	if(instances[entity->type]) {
		entity->next = instances[entity->type];
	}
	instances[entity->type] = entity;
}

static void CEntity_create(const char* name, EntityData* entity)
{
	if(!entity_registry[entity->type].construct)
		return;

	CEntity* obj = entity_registry[entity->type].construct(name, entity);
	if(obj) {
		CEntity_add(obj);
	}
}

void CEntity_initialize(Entity* entities, NODE* node)
{
	for(Entity* ent = entities; ent->data; ent++) {
		if(ent->data)
			CEntity_create(ent->node_name, ent->data);
	}
}

void CEntity_process(CEntity* entity, float dt)
{
	entity->funcs->process(entity, dt);
}

void CEntity_render(CEntity* entity)
{
	entity->funcs->render(entity);
}

void CEntity_process_all(float dt)
{
	for(int i = 0; i < class_count; i++) {
		if(entity_registry[i].process_class)
			entity_registry[i].process_class(dt);
		for(CEntity* ent = instances[i]; ent; ent = ent->next) {
			CEntity_process(ent, dt);
		}
	}
}

void EntSetTextureFilter(int type)
{
	for(int i = 0; i < class_count; i++) {
		if(entity_registry[i].set_tex_filter)
			entity_registry[i].set_tex_filter(type);
	}
}

extern float pos_x, pos_y, pos_z;

static float get_distance(Vec3* a, Vec3* b)
{
	float dx = a->x - b->x;
	float dy = a->y - b->y;
	float dz = a->z - b->z;

	return sqrtf(dx * dx + dy * dy + dz * dz);
}

static int CEntity_sort(const void* a, const void* b)
{
	Vec3 pos = { pos_x, pos_y, pos_z };

	CEntity* x = *(CEntity**) a;
	CEntity* y = *(CEntity**) b;

	Vec3* pos1 = x->funcs->get_position(x);
	Vec3* pos2 = y->funcs->get_position(y);
	float dst1 = get_distance(&pos, pos1);
	float dst2 = get_distance(&pos, pos2);
	if(dst1 == dst2) {
		return 0;
	} else if(dst1 < dst2) {
		return 1;
	} else {
		return -1;
	}
}

void CEntity_render_all(void)
{
	// sort entities back to front to avoid transparency issues

	int count = 0;
	for(int i = 0; i < class_count; i++) {
		for(CEntity* ent = instances[i]; ent; ent = ent->next) {
			count++;
		}
	}

	CEntity** tmp = (CEntity**) alloc_from_heap(count * sizeof(CEntity*));
	for(int i = 0, n = 0; i < class_count; i++) {
		for(CEntity* ent = instances[i]; ent; ent = ent->next) {
			tmp[n++] = ent;
		}
	}

	qsort(tmp, count, sizeof(CEntity*), CEntity_sort);

	for(int i = 0; i < count; i++) {
		CEntity_render(tmp[i]);
	}

	free_to_heap(tmp);
}

void get_transform_mtx(Mtx44* mtx, VecFx32* vec1, VecFx32* vec2)
{
	VecFx32 up;
	VecFx32 dir;

	VEC_CrossProduct(vec2, vec1, &up);
	VEC_Normalize(&up, &up);
	VEC_CrossProduct(vec1, &up, &dir);

	mtx->m[0][0] = FX_FX32_TO_F32(up.x);
	mtx->m[0][1] = FX_FX32_TO_F32(up.y);
	mtx->m[0][2] = FX_FX32_TO_F32(up.z);
	mtx->m[0][3] = 0;

	mtx->m[1][0] = FX_FX32_TO_F32(dir.x);
	mtx->m[1][1] = FX_FX32_TO_F32(dir.y);
	mtx->m[1][2] = FX_FX32_TO_F32(dir.z);
	mtx->m[1][3] = 0;

	mtx->m[2][0] = FX_FX32_TO_F32(vec1->x);
	mtx->m[2][1] = FX_FX32_TO_F32(vec1->y);
	mtx->m[2][2] = FX_FX32_TO_F32(vec1->z);
	mtx->m[2][3] = 0;

	mtx->m[3][0] = 0;
	mtx->m[3][1] = 0;
	mtx->m[3][2] = 0;
	mtx->m[3][3] = 1;
}
