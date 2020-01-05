#ifndef __GAME_H__
#define __GAME_H__

#include "types.h"
#include "strings.h"
#include "room.h"

#define	SINGLE_PLAYER	2

typedef struct {
	int	game_mode;
	int	room_id;
	int	area_id;
} State;

extern CRoom* room;
extern State game_state;

extern StringTable game_messages;
extern StringTable location_names;

void GAMEInit();
void GAMESetRoom(int room_id, unsigned int layer_mask);

const char* get_current_room_name();

void set_area_id();

#endif
