#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

#include "types.h"
#include "model.h"
#include "animation.h"
#include "entity.h"
#include "os.h"
#include "heap.h"

typedef struct {
	int	set_flag;
	char*	model_name;
	char*	anim_name;
	char*	collision_name;
	int	instant_sleep_anim_id;
	int	wakeup_anim_id;
	int	instant_wakeup_anim_id;
	int	sleep_anim_id;
	int	field_20;
	int	field_24;
} PlatformDescription;

#define	NUM_PLATFORMS	45

static PlatformDescription platforms[NUM_PLATFORMS] =
{
	{ 0, "platform", NULL, "platform", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "platform", NULL, "platform", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, NULL, NULL, NULL, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "Elevator", NULL, "Elevator", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "smasher", NULL, "smasher", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 1, "Platform_Unit4_C1", NULL, "Platform_Unit4_C1", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "pillar", NULL, "pillar", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "Door_Unit4_RM1", NULL, "Door_Unit4_RM1", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "SyluxShip", "SyluxShip", "SyluxShip", 4294967295, 1, 0, 2, 4294967295, 4294967295 },
	{ 0, "PistonMP7", NULL, "PistonMP7", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit3_brain", "Unit3_brain", NULL, 0, 0, 0, 0, 4294967295, 4294967295 },
	{ 0, "unit4_mover1", "unit4_mover1", "unit4_mover1", 0, 0, 0, 0, 0, 0 },
	{ 0, "unit4_mover2", "unit4_mover2", "unit4_mover2", 0, 0, 0, 0, 0, 0 },
	{ 0, "ElectroField1", "ElectroField1", "ElectroField1", 0, 0, 0, 0, 0, 0 },
	{ 0, "unit3_platform1", NULL, "unit3_platform1", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit3_pipe1", "unit3_pipe1", "unit3_pipe1", 0, 0, 0, 0, 0, 0 },
	{ 0, "unit3_pipe2", "unit3_pipe2", "unit3_pipe2", 0, 0, 0, 0, 0, 0 },
	{ 0, "CylinderBase", NULL, "CylinderBase", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit3_platform", NULL, "unit3_platform", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit3_platform2", NULL, "unit3_platform2", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit3_jar", "unit3_jar", NULL, 0, 2, 1, 0, 0, 0 },
	{ 0, "SyluxTurret", "SyluxTurret", NULL, 3, 2, 1, 0, 0, 0 },
	{ 0, "unit3_jartop", "unit3_jartop", NULL, 0, 2, 1, 0, 0, 0 },
	{ 0, "SamusShip", "SamusShip", "SamusShip", 1, 3, 2, 4, 0, 0 },
	{ 0, "unit1_land_plat1", NULL, "unit1_land_plat1", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit1_land_plat2", NULL, "unit1_land_plat2", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit1_land_plat3", NULL, "unit1_land_plat3", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit1_land_plat4", NULL, "unit1_land_plat4", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit1_land_plat5", NULL, "unit1_land_plat5", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit2_c4_plat", NULL, "unit2_c4_plat", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit2_land_elev", NULL, "unit2_land_elev", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit4_platform1", NULL, "unit4_platform1", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "crate01", "crate01", "crate01", 4294967295, 4294967295, 0, 1, 4294967295, 4294967295 },
	{ 0, "unit1_mover1", "unit1_mover1", "unit1_mover1", 0, 0, 0, 0, 0, 0 },
	{ 0, "unit1_mover2", NULL, "unit1_mover2", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit2_mover1", NULL, "unit2_mover1", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit4_mover3", NULL, "unit4_mover3", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit4_mover4", NULL, "unit4_mover4", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit3_mover1", NULL, "unit3_mover1", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit2_c1_mover", NULL, "unit2_c1_mover", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit3_mover2", "unit3_mover2", "unit3_mover2", 0, 0, 0, 0, 0, 0 },
	{ 0, "piston_gorealand", NULL, "piston_gorealand", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit4_tp2_artifact_wo", NULL, "unit4_tp2_artifact_wo", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "unit4_tp1_artifact_wo", NULL, "unit4_tp1_artifact_wo", 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295 },
	{ 0, "SamusShip", "SamusShip", "SamusShip", 1, 0, 2, 4, 0, 0 }
};

static CModel* platform_model[NUM_PLATFORMS] = { 0 };
static CAnimation* platform_anim[NUM_PLATFORMS] = { 0 };

void load_platform(int type)
{
	char filename[64];
	if(!platform_model[type]) {
		if(platforms[type].model_name) {
			sprintf(filename, "models/%s_Model.bin", platforms[type].model_name);
			load_model(&platform_model[type], filename, 0);
			if(platforms[type].anim_name) {
				sprintf(filename, "models/%s_Anim.bin", platforms[type].anim_name);
				load_animation(&platform_anim[type], filename, platform_model[type], 0);
			}
		}
	}
}

CEntity* CPlatform_construct(const char* node_name, EntityData* data)
{
	if(data->type != PLATFORM)
		OS_Terminate();

	EntityPlatform* platform = (EntityPlatform*)data;

	printf("Platform: %d\n", platform->type);

	CPlatform* obj = (CPlatform*)alloc_from_heap(sizeof(CPlatform));
	CEntityCtor(&obj->base, data);

	obj->type = platform->type;
	load_platform(obj->type);

	obj->pos.x = FX_FX32_TO_F32(platform->pos.x);
	obj->pos.y = FX_FX32_TO_F32(platform->pos.y);
	obj->pos.z = FX_FX32_TO_F32(platform->pos.z);

	get_transform_mtx(&obj->transform, &platform->vec2, &platform->vec1);
	obj->transform.m[3][0] = obj->pos.x;
	obj->transform.m[3][1] = obj->pos.y;
	obj->transform.m[3][2] = obj->pos.z;

	return (CEntity*)obj;
}

void CPlatform_process_class(float dt)
{
	for(int i = 0; i < NUM_PLATFORMS; i++) {
		if(platform_anim[i])
			CAnimation_process(platform_anim[i], dt);
	}
}

void CPlatform_render(CEntity* obj)
{
	CPlatform* self = (CPlatform*)obj;

	if(!platform_model[self->type])
		return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(self->transform.a);

	CModel_render_all(platform_model[self->type]);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

Vec3* CPlatform_get_position(CEntity* obj)
{
	CPlatform* self = (CPlatform*)obj;

	return &self->pos;
}

void CPlatform_set_tex_filter(int type)
{
	unsigned int i;
	for(i = 0; i < NUM_PLATFORMS; i++) {
		if(platform_model[i])
			CModel_set_texture_filter(platform_model[i], type);
	}
}

void EntPlatformRegister(void)
{
	EntityClass* ent = EntRegister(PLATFORM);
	ent->construct = CPlatform_construct;
	ent->process_class = CPlatform_process_class;
	ent->render = CPlatform_render;
	ent->get_position = CPlatform_get_position;
	ent->set_tex_filter = CPlatform_set_tex_filter;
}
