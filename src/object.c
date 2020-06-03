#include <stdio.h>
#include <string.h>

#include "types.h"
#include "model.h"
#include "animation.h"
#include "texture_containers.h"
#include "entity.h"
#include "os.h"

#define NUM_OBJECTS	54

static ObjectDescription objects[NUM_OBJECTS] =
{
	{ 0, "AlimbicGhost_01", "AlimbicGhost_01", NULL, { 0, 0, 0, 0 } },
	{ 0, "AlimbicLightPole", "AlimbicLightPole", "AlimbicLightPole", { 0, 0, 0, 0 } },
	{ 0, "AlimbicStationShieldControl", "AlimbicStationShieldControl", NULL, { 0, 0, 0, 0 } },
	{ 0, "AlimbicComputerStationControl", "AlimbicComputerStationControl", NULL, { 0, 0, 0, 0 } },
	{ 0, "AlimbicEnergySensor", "AlimbicEnergySensor", NULL, { 0, 0, 0, 0 } },
	{ 0, "SamusShip", "SamusShip", "SamusShip", { 0, 0, 0, 0 } },
	{ 0, "Guardbot01_Dead", NULL, NULL, { 0, 0, 0, 0 } },
	{ 0, "Guardbot02_Dead", NULL, NULL, { 0, 0, 0, 0 } },
	{ 0, "Guardian_Dead", NULL, NULL, { 0, 0, 0, 0 } },
	{ 0, "Psychobit_Dead", NULL, NULL, { 0, 0, 0, 0 } },
	{ 0, "AlimbicLightPole02", "AlimbicLightPole02", "AlimbicLightPole02", { 0, 0, 0, 0 } },
	{ 0, "AlimbicComputerStationControl02", "AlimbicComputerStationControl02", NULL, { 0, 0, 0, 0 } },
	{ 0, "Generic_Console_mdl", "Generic_Console", "Generic_Console", { 2, 1, 0, 0 } },
	{ 0, "Generic_Monitor_mdl", "Generic_Monitor", "Generic_Monitor", { 2, 1, 0, 0 } },
	{ 0, "Generic_Power_mdl", "Generic_Power", NULL, { 0, 0, 0, 0 } },
	{ 0, "Generic_Scanner_mdl", "Generic_Scanner", NULL, { 2, 1, 0, 0 } },
	{ 1, "Generic_Switch_mdl", "Generic_Switch", "Generic_Switch", { 2, 1, 0, 0 } },
	{ 0, "Alimbic_Console_mdl", "Alimbic_Console", "Alimbic_Console", { 2, 1, 0, 0 } },
	{ 0, "Alimbic_Monitor_mdl", "Alimbic_Monitor", "Alimbic_Monitor", { 2, 1, 0, 0 } },
	{ 0, "Alimbic_Power_mdl", "Alimbic_Power", NULL, { 0, 0, 0, 0 } },
	{ 0, "Alimbic_Scanner_mdl", "Alimbic_Scanner", NULL, { 2, 1, 0, 0 } },
	{ 1, "Alimbic_Switch_mdl", "Alimbic_Switch", "Alimbic_Switch", { 2, 1, 0, 0 } },
	{ 0, "Lava_Console_mdl", "Lava_Console", "Lava_Console", { 2, 1, 0, 0 } },
	{ 0, "Lava_Monitor_mdl", "Lava_Monitor", "Lava_Monitor", { 2, 1, 0, 0 } },
	{ 0, "Lava_Power_mdl", "Lava_Power", NULL, { 0, 0, 0, 0 } },
	{ 0, "Lava_Scanner_mdl", "Lava_Scanner", NULL, { 2, 1, 0, 0 } },
	{ 1, "Lava_Switch_mdl", "Lava_Switch", "Lava_Switch", { 2, 1, 0, 0 } },
	{ 0, "Ice_Console_mdl", "Ice_Console", "Ice_Console", { 2, 1, 0, 0 } },
	{ 0, "Ice_Monitor_mdl", "Ice_Monitor", "Ice_Monitor", { 2, 1, 0, 0 } },
	{ 0, "Ice_Power_mdl", "Ice_Power", NULL, { 0, 0, 0, 0 } },
	{ 0, "Ice_Scanner_mdl", "Ice_Scanner", NULL, { 2, 1, 0, 0 } },
	{ 1, "Ice_Switch_mdl", "Ice_Switch", "Ice_Switch", { 2, 1, 0, 0 } },
	{ 0, "Ruins_Console_mdl", "Ruins_Console", "Ruins_Console", { 2, 1, 0, 0 } },
	{ 0, "Ruins_Monitor_mdl", "Ruins_Monitor", "Ruins_Monitor", { 2, 1, 0, 0 } },
	{ 0, "Ruins_Power_mdl", "Ruins_Power", NULL, { 0, 0, 0, 0 } },
	{ 0, "Ruins_Scanner_mdl", "Ruins_Scanner", NULL, { 2, 1, 0, 0 } },
	{ 1, "Ruins_Switch_mdl", "Ruins_Switch", "Ruins_Switch", { 2, 1, 0, 0 } },
	{ 0, "PlantCarnivarous_Branched", "PlantCarnivarous_Branched", NULL, { 0, 0, 0, 0 } },
	{ 0, "PlantCarnivarous_Pod", "PlantCarnivarous_Pod", NULL, { 0, 0, 0, 0 } },
	{ 0, "PlantCarnivarous_PodLeaves", "PlantCarnivarous_PodLeaves", NULL, { 0, 0, 0, 0 } },
	{ 0, "PlantCarnivarous_Vine", "PlantCarnivarous_Vine", NULL, { 0, 0, 0, 0 } },
	{ 0, "GhostSwitch_mdl", "GhostSwitch_mdl", NULL, { 0, 0, 0, 0 } },
	{ 1, "Switch_mdl", NULL, NULL, { 0, 0, 0, 0 } },
	{ 0, "Guardian_Stasis", "Guardian_Stasis", NULL, { 0xFF, 0, 0, 0 } },
	{ 0, "AlimbicStatue_lod0", NULL, "AlimbicStatue", { 0xFF, 0, 0, 0 } },
	{ 0, "AlimbicCapsule", "AlimbicCapsule", "AlimbicCapsule", { 0, 0, 0, 0 } },
	{ 1, "SniperTarget", "SniperTarget", NULL, { 0, 2, 1, 0 } },
	{ 0, "SecretSwitch_pal_01", "SecretSwitch", "SecretSwitch", { 1, 2, 0, 0 } },
	{ 0, "SecretSwitch_pal_02", "SecretSwitch", "SecretSwitch", { 1, 2, 0, 0 } },
	{ 0, "SecretSwitch_pal_03", "SecretSwitch", "SecretSwitch", { 1, 2, 0, 0 } },
	{ 0, "SecretSwitch_pal_04", "SecretSwitch", "SecretSwitch", { 1, 2, 0, 0 } },
	{ 0, "SecretSwitch_pal_05", "SecretSwitch", "SecretSwitch", { 1, 2, 0, 0 } },
	{ 0, "SecretSwitch_pal_06", "SecretSwitch", "SecretSwitch", { 1, 2, 0, 0 } },
	{ 1, "WallSwitch", "WallSwitch", NULL, { 2, 0, 1, 0 } }
};

CModel* object_model[NUM_OBJECTS];
CAnimation* object_anim[NUM_OBJECTS];

void link_model(CModel* model, SHARED_TEXTURE container_id, int copy_textures)
{
	TEXTURE* tex = model->textures;
	unsigned int texcnt = model->num_textures;

	if(!texture_containers[container_id])
		load_model(&texture_containers[container_id], texture_container_names[container_id], 0);

	memcpy(model, texture_containers[container_id], sizeof(CModel));

	if(!copy_textures) {
		model->textures = tex;
		model->num_textures = texcnt;
	}
}

void load_object(unsigned int id)
{
	const char* model_name;
	CModel* mdl;
	char filename[64];

	if(!object_model[id] && objects[id].model_name) {
		sprintf(filename, "models/%s_Model.bin", objects[id].model_name);
		load_model(&object_model[id], filename, 0);
		// if(objects[id].unk_flag)
		// 	object_model[id]->flags |= 1u;
		if(id == 41) {
			if(object_model[41]->textures)
				OS_Terminate();
			load_texture_container(object_model[41], ALIMBIC_TXTR);
		} else if(id >= 0xC && id <= 0x10) {
			mdl = object_model[id];
			if(mdl->textures)
				OS_Terminate();
			load_texture_container(object_model[id], GENERIC_EQUIP_TXTR);
		} else if(id >= 0x11 && id <= 0x15) {
			mdl = object_model[id];
			if(mdl->textures)
				OS_Terminate();
			load_texture_container(object_model[id], ALIMBIC_EQUIP_TXTR);
		} else if(id >= 0x16 && id <= 0x1A) {
			mdl = object_model[id];
			if(mdl->textures)
				OS_Terminate();
			load_texture_container(object_model[id], LAVA_EQUIP_TXTR);
		} else if(id >= 0x1B && id <= 0x1F) {
			mdl = object_model[id];
			if(mdl->textures)
				OS_Terminate();
			load_texture_container(object_model[id], ICE_EQUIP_TXTR);
		} else if(id >= 0x20 && id <= 0x24) {
			mdl = object_model[id];
			if(mdl->textures)
				OS_Terminate();
			load_texture_container(object_model[id], RUINS_EQUIP_TXTR);
		} else if(id >= 0x2F && id <= 0x34) {
			link_model(object_model[id], SECRET_SWITCH_TXTR, 1);
		} else if(id == 45) {
			// load_collision(&alimbic_capsule_shield_collision, "models/AlmbCapsuleShld_Collision.bin", 0);
			object_model[45]->texture_matrices->m[0][0] = 0;
			object_model[45]->texture_matrices->m[0][1] = 0;
			object_model[45]->texture_matrices->m[0][2] = 0;
			object_model[45]->texture_matrices->m[1][0] = -2048;
			object_model[45]->texture_matrices->m[1][1] = 0;
			object_model[45]->texture_matrices->m[1][2] = 0;
			object_model[45]->texture_matrices->m[2][0] = 410;
			object_model[45]->texture_matrices->m[2][1] = -3891;
			object_model[45]->texture_matrices->m[2][2] = 0;
			object_model[45]->texture_matrices->m[3][0] = 0;
			object_model[45]->texture_matrices->m[3][1] = 0;
			object_model[45]->texture_matrices->m[3][2] = 0;
		}
		if(objects[id].anim_name && !object_anim[id]) {
			sprintf(filename, "models/%s_Anim.bin", objects[id].anim_name);
			load_animation(&object_anim[id], filename, object_model[id], 0);
		}
		// if(objects[id].collision_name && !object_collision[id]) {
		// 	sprintf(filename, "models/%s_Collision.bin", objects[id].collision_name);
		// 	load_collision(&object_collision[id], filename, 0);
		// }
		if(id >= 0x2F && id <= 0x34) {
			int i;
			for(i = 47; i < 0x34; i = (i + 1) & 0xFF) {
				object_anim[i] = object_anim[id];
				// object_collision[i] = object_collision[id];
			}
		}
	}
}
