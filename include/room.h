#ifndef __ROOM_H__
#define __ROOM_H__

#include "types.h"
#include "model.h"
#include "animation.h"
#include "rooms.h"

typedef struct {
	char			name[32];
	int			layer_mask;
	MtxFx43			transform;
	VecFx32			pos;
	CModel*			model;
	CAnimation*		animation;
	const RoomDescription*	description;
} CRoom;

CRoom*	load_room(const RoomDescription* descr, fx32 x, fx32 y, fx32 z, int layer_mask);
void	CRoom_render(CRoom* room);
void	CRoom_process(CRoom* room, float dt);

#endif
