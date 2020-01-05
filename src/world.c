#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "strings.h"
#include "room.h"
#include "rooms.h"
#include "game.h"
#include "world.h"
#include "heap.h"

CPortal* portals = NULL;
int portal_count = 0;

RoomInfo* current_room_info;

void setup_room_portals()
{
	int i;
	RoomInfo* prev_room_info = NULL;
	fx32 v4;
	char buf[5];
	char room_name[16];
	char ent_filename[128];

	portals = (CPortal *)alloc_from_heap(128 * sizeof(CPortal));
	portal_count = 0;

	for(i = 0; i < 36; i++) {
		const char* location_name;
		sprintf(buf, "R%d%02d", game_state.area_id >> 1, i + 1);
		location_name = get_string_by_id(&location_names, buf);
		if(!location_name)
			break;

		if(strncmp(location_name, "Con", 3)) {
			RoomInfo* room_info;
			int room_id;

			const char* last_backslash = strrchr(location_name, '\\');
			int name_length = (int) (last_backslash - location_name);
			strncpy(room_name, location_name, (int) (last_backslash - location_name));
			room_name[name_length] = 0;

			sprintf(ent_filename, "levels/entities/%s_Ent.bin", room_name);
			room_info = (RoomInfo*) alloc_from_heap(sizeof(RoomInfo));
			sprintf(room_info->name, "%s", room_name);
			room_info->map_name = last_backslash + 1;

			room_info->portal = NULL;
			for(room_id = 0; room_id < NUM_ROOMS; room_id++) {
				if(!strcasecmp(rooms[room_id].name, room_name)) {
					room_info->room_id = room_id;
					break;
				}
			}

			room_info->next = prev_room_info;
			prev_room_info = room_info;

			v4 = 0x1000;
			if(!strcasecmp(room_name, "UNIT2_C2")) {
				v4 = 0x20CC;
			} else if(!strcasecmp(room_name, "UNIT2_C3")) {
				v4 = 0x14E1;
			}
			// parse_door_and_teleporter_entities(ent_filename, room_name_info, ((unsigned int)something_with_layer_id >> 2 * game_state.area_id) & 3, v4);
		}
	}

	current_room_info = prev_room_info;
	// game_state.some_flags = game_state.some_flags & 0x9FFF | 0x4000;
}

const char* get_room_name(int room_id)
{
	RoomInfo* info;
	for(info = current_room_info; info; info = info->next) {
		if(info->room_id == room_id)
			return info->map_name;
	}
	return NULL;
}
