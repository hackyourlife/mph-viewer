#ifndef __WORLD_H__
#define __WORLD_H__

typedef struct cportal CPortal;

struct cportal {
	unsigned short		id;
	unsigned short		palette_id;
	unsigned short		target_layer_id;
	VecFx32			pos;
	CPortal*		next;
};

typedef struct room_info RoomInfo;

struct room_info {
	char			name[32];
	const char*		map_name;
	int			room_id;
	CPortal*		portal;
	RoomInfo*		next;
};

void setup_room_portals();
const char* get_room_name(int room_id);

#endif
