#include <GL/gl.h>
#include <GL/glext.h>

#include <stdlib.h>

#include "types.h"
#include "mtx.h"
#include "heap.h"
#include "room.h"
#include "animation.h"
#include "io.h"
#include "entity.h"
#include "model.h"
#include "archive.h"

Entity* entities;

CRoom* load_room(const RoomDescription* descr, fx32 x, fx32 y, fx32 z, int layer_mask)
{
	int i;
	char filename[256];
	char txtrfilename[256];

	CRoom* room = (CRoom*)alloc_from_heap(sizeof(CRoom));
	room->pos.x = x;
	room->pos.y = y;
	room->pos.z = z;

	LoadArchive(descr->archive_name, descr->archive);

	room->description = descr;
	if(descr->layer_id) {
		room->layer_mask = (layer_mask & 0xC03F) | (((1 << descr->layer_id) & 0xFF) << 6);
	} else {
		room->layer_mask = 0xFFFFFFFF;
	}

	if(layer_mask) { // override layer mask
		room->layer_mask = layer_mask;
	}

	sprintf(room->name, "%s", descr->name);

	int flags = USE_ARCHIVE;
	sprintf(filename, "%s/%s", descr->archive_name, descr->model);
	if(descr->tex) {
		flags = 0x20 | USE_EXTERNAL_TXTR | USE_ARCHIVE;
		sprintf(txtrfilename, "levels/textures/%s", descr->tex);
		load_room_model(&room->model, filename, txtrfilename, flags, room->layer_mask);
	} else {
		load_room_model(&room->model, filename, NULL, flags, room->layer_mask);
	}
	// room->model->apply_transform = 0;

	if(descr->anim) {
		sprintf(filename, "%s/%s", descr->archive_name, descr->anim);
		load_animation(&room->animation, filename, room->model, USE_ARCHIVE);
	}

	if(descr->ent) {
		int layer_id = 0;
		sprintf(filename, "levels/entities/%s", descr->ent);
		EntLoad(&entities, filename, layer_id);

		CEntity_initialize(entities, room->model->nodes);
	}

	// setup node refs
	room->room_nodes = NULL;
	for(i = 0; i < room->model->num_nodes; i++) {
		CNode* node = &room->model->nodes[i];
		if(node->name[0] == 'r' && node->name[1] == 'm') {
			NodeRef* ref = (NodeRef*) malloc(sizeof(NodeRef));
			ref->node_id = node->child;
			ref->next = room->room_nodes;
			room->room_nodes = ref;
		}
	}

	return room;
}

void CRoom_free(CRoom* room)
{
	if(room->model) {
		CModel_free(room->model);
		room->model = NULL;
	}
	if(room->animation) {
		CAnimation_free(room->animation);
		room->animation = NULL;
	}
	if(room->room_nodes) {
		NodeRef* ref = room->room_nodes;
		while(ref) {
			NodeRef* next = ref->next;
			free(ref);
			ref = next;
		}
		room->room_nodes = NULL;
	}
}

void CRoom_setLights(CRoom* room)
{
	float light_1_vec[3] = { FX_FX32_TO_F32(room->description->light_1_vec.x), FX_FX32_TO_F32(room->description->light_1_vec.y), FX_FX32_TO_F32(room->description->light_1_vec.z) };
	float light_1_col[3] = { room->description->light_1_color.r / 255.0f, room->description->light_1_color.g / 255.0f, room->description->light_1_color.b / 255.0f };
	float light_2_vec[3] = { FX_FX32_TO_F32(room->description->light_2_vec.x), FX_FX32_TO_F32(room->description->light_2_vec.y), FX_FX32_TO_F32(room->description->light_2_vec.z) };
	float light_2_col[3] = { room->description->light_2_color.r / 255.0f, room->description->light_2_color.g / 255.0f, room->description->light_2_color.b / 255.0f };

#if 0
	glLightfv(GL_LIGHT0, GL_POSITION, light_1_vec);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_1_col);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_1_col);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_1_col);
	glLightfv(GL_LIGHT1, GL_POSITION, light_2_vec);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_2_col);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_2_col);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_2_col);
#endif

	CModel_setLights(light_1_vec, light_1_col, light_2_vec, light_2_col);

#if 0
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
#endif
}

void CRoom_render(CRoom* room)
{
	NodeRef* ref;
	Mtx44 mtx;
	float fogcolor[4] = { COLOR_R(room->description->fog_color), COLOR_G(room->description->fog_color), COLOR_B(room->description->fog_color), 1 };
	MTX44Trans(&mtx, FX_FX32_TO_F32(room->pos.x), FX_FX32_TO_F32(room->pos.y), FX_FX32_TO_F32(room->pos.z));
	CRoom_setLights(room);
	CModel_setFog(room->description->fog_enable, fogcolor, room->description->fog_offset & 0x7FFF, room->description->fog_slope);
	for(ref = room->room_nodes; ref; ref = ref->next) {
		CModel_render_node(room->model, &mtx, ref->node_id, 1.0);
	}
}

void CRoom_process(CRoom* room, float dt)
{
	if(room->model->animation)
		CAnimation_process(room->model->animation, dt);
}
