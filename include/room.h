#ifndef __ROOM_H__
#define __ROOM_H__

#include "types.h"
#include "model.h"
#include "animation.h"
#include "rooms.h"

typedef struct NodeRef NodeRef;

typedef struct {
	char			name[32];
	int			layer_mask;
	MtxFx43			transform;
	VecFx32			pos;
	CModel*			model;
	CAnimation*		animation;
	const RoomDescription*	description;
	NodeRef*		room_nodes;
} CRoom;

struct NodeRef {
	int			node_id;
	NodeRef*		next;
};

CRoom*	load_room(const RoomDescription* descr, fx32 x, fx32 y, fx32 z, int layer_mask);
void	CRoom_render(CRoom* room);
void	CRoom_process(CRoom* room, float dt);
void	CRoom_free(CRoom* room);

#endif
