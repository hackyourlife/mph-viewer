#include <stdio.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "types.h"
#include "model.h"
#include "animation.h"
#include "texture_containers.h"
#include "entity.h"
#include "game.h"
#include "os.h"
#include "heap.h"

static CModel* teleporter_model = NULL;
static CModel* teleporter_small_model = NULL;
static CModel* teleporter_pal_model = NULL;

static CAnimation* teleporter_anim = NULL;
static CAnimation* teleporter_small_anim = NULL;

static void load_teleporter_model(void)
{
	char filename[64];

	if(game_state.game_mode == SINGLE_PLAYER) {
		if(!teleporter_pal_model) {
			sprintf(filename, "models/Teleporter_pal_%02d_Model.bin", game_state.area_id + 1);
			printf("Loading palette model %s...\n", filename);
			load_model(&teleporter_pal_model, filename, 0);
			if(teleporter_pal_model->dlists || teleporter_pal_model->textures)
				OS_Terminate();
		}

		if(!teleporter_small_model) {
			load_model(&teleporter_small_model, "models/TeleporterSmall_mdl_Model.bin", 0);
			if(teleporter_small_model->textures || teleporter_small_model->palettes)
				OS_Terminate();
			load_texture_container(teleporter_small_model, TELEPORTER_TXTR);
			if(!teleporter_small_anim)
				load_animation(&teleporter_small_anim, "models/TeleporterSmall_mdl_Anim.bin", teleporter_small_model, 0);
			if(teleporter_pal_model) {
				teleporter_small_model->palettes = teleporter_pal_model->palettes;
				teleporter_small_model->num_palettes = teleporter_pal_model->num_palettes;
			}
			CModel_set_textures(teleporter_small_model);
		}

		if(!teleporter_model) {
			load_model(&teleporter_model, "models/Teleporter_mdl_Model.bin", 0);
			if(teleporter_model->textures || teleporter_model->palettes)
				OS_Terminate();
			load_texture_container(teleporter_model, TELEPORTER_TXTR);
			if(!teleporter_anim)
				load_animation(&teleporter_anim, "models/Teleporter_mdl_Anim.bin", teleporter_model, 0);
			if(teleporter_pal_model) {
				teleporter_model->palettes = teleporter_pal_model->palettes;
				teleporter_model->num_palettes = teleporter_pal_model->num_palettes;
			}
			CModel_set_textures(teleporter_model);
		}
	} else {
		load_model(&teleporter_model, "models/TeleporterMP_Model.bin", 0);
		if(!teleporter_anim)
			load_animation(&teleporter_anim, "models/TeleporterMP_Anim.bin", teleporter_model, 0);
		teleporter_small_model = teleporter_model;
		teleporter_small_anim = teleporter_anim;
		CModel_set_textures(teleporter_model);
	}
}

CEntity* CTeleporter_construct(const char* node_name, EntityData* data)
{
	if(data->type != TELEPORTER)
		OS_Terminate();

	EntityTeleporter* teleporter = (EntityTeleporter*)data;

	CTeleporter* obj = (CTeleporter*)alloc_from_heap(sizeof(CTeleporter));
	CEntityCtor(&obj->base, data);

	obj->flags = (teleporter->artifact_id < 8 && !teleporter->invisible) ? 2 : 0;

	printf("Teleporter flags: %d\n", obj->flags);

	if(teleporter->invisible) {
		obj->model = NULL;
		obj->base.scan_id = 0;
	} else {
		obj->model = !(obj->flags & 2) ? teleporter_small_model : teleporter_model;
	}

	obj->pos.x = FX_FX32_TO_F32(teleporter->pos.x);
	obj->pos.y = FX_FX32_TO_F32(teleporter->pos.y);
	obj->pos.z = FX_FX32_TO_F32(teleporter->pos.z);

	get_transform_mtx(&obj->transform, &teleporter->vec2, &teleporter->vec1);
	obj->transform.m[3][0] = obj->pos.x;
	obj->transform.m[3][1] = obj->pos.y;
	obj->transform.m[3][2] = obj->pos.z;

	return (CEntity*)obj;
}

void CTeleporter_process_class(float dt)
{
	if(teleporter_anim)
		CAnimation_process(teleporter_anim, dt);
	if(teleporter_small_anim)
		CAnimation_process(teleporter_small_anim, dt);
}

void CTeleporter_render(CEntity* obj)
{
	CTeleporter* self = (CTeleporter*)obj;

	if(!self->model)
		return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(self->transform.a);

	CModel_render_all(self->model);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

Vec3* CTeleporter_get_position(CEntity* obj)
{
	CTeleporter* self = (CTeleporter*)obj;

	return &self->pos;
}

void CTeleporter_set_tex_filter(int type)
{
	if(teleporter_model)
		CModel_set_texture_filter(teleporter_model, type);
	if(teleporter_small_model)
		CModel_set_texture_filter(teleporter_small_model, type);
}

void EntTeleporterRegister(void)
{
	EntityClass* ent = EntRegister(TELEPORTER);
	ent->construct = CTeleporter_construct;
	ent->process_class = CTeleporter_process_class;
	ent->render = CTeleporter_render;
	ent->get_position = CTeleporter_get_position;
	ent->set_tex_filter = CTeleporter_set_tex_filter;

	load_teleporter_model();
}
