#ifndef __ROOMS_H__
#define __ROOMS_H__

#include "types.h"

#define NUM_ROOMS 122

typedef struct {
	char*	name;
	char*	model;
	char*	anim;
	char*	tex;
	char*	collision;
	char*	ent;
	char*	node;
	char*	room_node_name;
	u32	battle_time_limit;
	u32	time_limit;
	s16	point_limit;
	s16	layer_id;
	s32	far_clip_dist;
	u16	fog_enable;
	u16	clear_fog;
	u16	fog_color;
	s16	field_36;
	u32	fog_slope;
	u32	fog_offset;
	Color4	light_1_color;
	VecFx32	light_1_vec;
	Color4	light_2_color;
	VecFx32	light_2_vec;
	char*	archive_name;
	char*	archive;
	u32	unk21;
	u32	unk22;
} RoomDescription;

extern const RoomDescription rooms[NUM_ROOMS];
extern const VecFx32 room_sizes[NUM_ROOMS];

#endif
