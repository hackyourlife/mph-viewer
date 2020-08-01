#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "types.h"
#include "model.h"
#include "animation.h"
#include "texture_containers.h"
#include "entity.h"
#include "os.h"
#include "heap.h"
#include "mtx.h"

#define	NUM_ARTIFACTS	8

static CModel* artifact_base_model = NULL;
static CAnimation* artifact_base_anim = NULL;

static CModel* octolith_model = NULL;
static CAnimation* octolith_anim = NULL;

static CModel* artifact_models[NUM_ARTIFACTS] = { 0 };
static CAnimation* artifact_anim[NUM_ARTIFACTS] = { 0 };

static void load_base(void)
{
	if(!artifact_base_model) {
		load_model(&artifact_base_model, "models/ArtifactBase_Model.bin", 0);
		if(!artifact_base_anim) {
			load_animation(&artifact_base_anim, "models/ArtifactBase_Anim.bin", artifact_base_model, 0);
		}
	}
}

static void load_artifact(unsigned int artifact_id)
{
	CModel* artifact_model;
	char filename[64];

	if(artifact_id >= 8) {
		if(!octolith_model) {
			load_model(&octolith_model, "models/Octolith_Model.bin", 0);
			load_animation(&octolith_anim, "models/Octolith_Anim.bin", octolith_model, 0);
		}
	} else if(!artifact_models[artifact_id]) {
		sprintf(filename, "models/Artifact%02d_mdl_Model.bin", artifact_id + 1);
		load_model(&artifact_models[artifact_id], filename, 0);
		artifact_model = artifact_models[artifact_id];
		if(artifact_model->textures || artifact_model->palettes)
			OS_Terminate();
		load_texture_container(artifact_models[artifact_id], ARTIFACT_TXTR);
		if(!artifact_anim[artifact_id])
			load_animation(&artifact_anim[artifact_id], "models/Artifact_Anim.bin", artifact_models[artifact_id], 0);
	}
}

CEntity* CArtifact_construct(const char* node_name, EntityData* data)
{
	if(data->type != ARTIFACT)
		OS_Terminate();

	EntityArtifact* artifact = (EntityArtifact*) data;

	printf("Artifact: id=%d, model=%d, base=%d\n", artifact->artifact_id, artifact->model_id, artifact->has_base);

	CArtifact* self = (CArtifact*) alloc_from_heap(sizeof(CArtifact));
	CEntityCtor(&self->base, data);

	self->pos.x = FX_FX32_TO_F32(artifact->pos.x);
	self->pos.y = FX_FX32_TO_F32(artifact->pos.y);
	self->pos.z = FX_FX32_TO_F32(artifact->pos.z);

	get_transform_mtx(&self->transform, &artifact->vec2, &artifact->vec1);
	self->transform.m[3][0] = self->pos.x;
	self->transform.m[3][1] = self->pos.y;
	self->transform.m[3][2] = self->pos.z;
	self->base_transform = self->transform;

	self->model_id = artifact->model_id;
	self->artifact_id = artifact->artifact_id;
	self->has_base = artifact->has_base;

	if(self->has_base)
		load_base();

	load_artifact(self->model_id);

	if(self->model_id >= 8)
		self->transform.m[3][1] += 1.75; // CItem_get_y(octolith_model->nodes[0].offset_raw);
	else
		self->transform.m[3][1] += artifact_models[self->model_id]->nodes[0].offset;

	self->rotation = 0;

	return (CEntity*) self;
}

void CArtifact_process_class(float dt)
{
	for(int i = 0; i < NUM_ARTIFACTS; i++) {
		if(artifact_anim[i])
			CAnimation_process(artifact_anim[i], dt);
	}
	if(octolith_anim)
		CAnimation_process(octolith_anim, dt);
}

void CArtifact_process(CEntity* obj, float dt)
{
	CArtifact* self = (CArtifact*)obj;

	self->rotation += dt * 360.0 * 0.5;
	while(self->rotation > 360.0)
		self->rotation -= 360.0;
}

void CArtifact_render(CEntity* obj)
{
	CArtifact* self = (CArtifact*)obj;

	if(self->has_base) {
		CModel_render_all(artifact_base_model, &self->base_transform, 1.0);
	}

	if(self->model_id >= 8) {
		CModel_render_all(octolith_model, &self->transform, 1.0);
	} else {
		Mtx44 mtx;
		MTX44RotRad(&mtx, 'y', self->rotation / 180.0 * M_PI);
		MTX44Concat(&self->transform, &mtx, &mtx);

		CModel_render_all(artifact_models[self->model_id], &mtx, 1.0);
	}
}

Vec3* CArtifact_get_position(CEntity* obj)
{
	CArtifact* self = (CArtifact*)obj;

	return &self->pos;
}

void CArtifact_set_tex_filter(int type)
{
	unsigned int i;
	for(i = 0; i < NUM_ARTIFACTS; i++) {
		if(artifact_models[i])
			CModel_set_texture_filter(artifact_models[i], type);
	}
	if(artifact_base_model)
		CModel_set_texture_filter(artifact_base_model, type);
	if(octolith_model)
		CModel_set_texture_filter(octolith_model, type);
}

void EntArtifactRegister(void)
{
	EntityClass* ent = EntRegister(ARTIFACT);
	ent->construct = CArtifact_construct;
	ent->process_class = CArtifact_process_class;
	ent->process = CArtifact_process;
	ent->render = CArtifact_render;
	ent->get_position = CArtifact_get_position;
	ent->set_tex_filter = CArtifact_set_tex_filter;
}
