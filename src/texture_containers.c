#include "os.h"
#include "texture_containers.h"

const char* texture_container_names[NUM_TEXTURE_CONTAINERS] = {
		"models/AlimbicTextureShare_img_Model.bin"
		"models/TeleporterTextureShare_img_Model.bin"
		"models/ArtifactTextureShare_img_Model.bin"
		"models/AlimbicEquipTextureShare_img_Model.bin"
		"models/IceEquipTextureShare_img_Model.bin",
		"models/LavaEquipTextureShare_img_Model.bin",
		"models/RuinsEquipTextureShare_img_Model.bin",
		"models/GenericEquipTextureShare_img_Model.bin",
		"models/PsychoBit_img_00_Model.bin",
		"models/PsychoBit_img_01_Model.bin",
		"models/PsychoBit_img_02_Model.bin",
		"models/PsychoBit_img_03_Model.bin",
		"models/PsychoBit_img_04_Model.bin",
		"models/GuardBot1_img_00_Model.bin",
		"models/GuardBot1_img_01_Model.bin",
		"models/GuardBot1_img_02_Model.bin",
		"models/GuardBot1_img_03_Model.bin",
		"models/GuardBot1_img_04_Model.bin",
		"models/Alimbic_Turret_img_00_Model.bin",
		"models/Alimbic_Turret_img_04_Model.bin",
		"models/Alimbic_Turret_img_05_Model.bin",
		"models/LavaDemon_img_00_Model.bin",
		"models/LavaDemon_img_03_Model.bin",
		"models/BarbedWarWasp_img_00_Model.bin",
		"models/BarbedWarWasp_img_02_Model.bin",
		"models/BarbedWarWasp_img_03_Model.bin",
		"models/SecretSwitch_Model.bin"
};

CModel* texture_containers[NUM_TEXTURE_CONTAINERS];

void load_texture_container(CModel* model, SHARED_TEXTURE id)
{
	if(!texture_containers[id]) {
		load_model(&texture_containers[id], texture_container_names[id], 0);
		if(texture_containers[id]->dlists)
			OS_Terminate();
	}

	model->num_textures = texture_containers[id]->num_textures;
	model->textures = texture_containers[id]->textures;
}
