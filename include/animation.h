#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "types.h"
#include "model.h"

void load_animation(CAnimation** animation, const char* filename, CModel* model, char flags);
void CAnimation_free(CAnimation* animation);

void CAnimation_process(CAnimation* animation, float dt);
void process_texcoord_animation(CTexcoordAnimationGroup* group, int id, int width, int height, Mtx44* texcoord);
void process_material_animation(CMaterialAnimationGroup* group, int id, CMaterial* material);
void process_node_animation(CNodeAnimationGroup* group, Mtx44* root_transform, float scale);

#endif
