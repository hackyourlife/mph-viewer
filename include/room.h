#pragma once

#include "model.h"
#include "rooms.h"

typedef struct {
	char			name[32];
	int			layer_mask;
	MtxFx43			transform;
	VecFx32			pos;
	CModel*			model;
	const RoomDescription*	description;
} CRoom;

CRoom*	load_room(const RoomDescription* descr, int layer_mask);
void	CRoom_render(CRoom* room);
