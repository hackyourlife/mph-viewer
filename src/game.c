#include "types.h"
#include "game.h"
#include "entity.h"
#include "archive.h"
#include "strings.h"
#include "hud.h"
#include "world.h"

CRoom* room;
State game_state = { 0 };

void GAMEInit()
{
	game_state.game_mode = SINGLE_PLAYER;

	LoadArchive("common", "archives/common.arc");

	load_hud_messages();
	load_game_messages();
	load_weapon_names();
}

void GAMESetRoom(int room_id, unsigned int layer_mask)
{
	game_state.room_id = room_id;
	set_area_id();
	EntInitialize(28);
	room = load_room(&rooms[room_id], 0, 0, 0, layer_mask);
	setup_room_portals();
}

const char* get_current_room_name()
{
	return get_room_name(game_state.room_id);
}

void set_area_id()
{
	if(game_state.room_id >= 27 && game_state.room_id < 36)
		game_state.area_id = 0;
	else if(game_state.room_id >= 36 && game_state.room_id < 45)
		game_state.area_id = 1;
	else if(game_state.room_id >= 45 && game_state.room_id < 56)
		game_state.area_id = 2;
	else if(game_state.room_id >= 56 && game_state.room_id < 65)
		game_state.area_id = 3;
	else if(game_state.room_id >= 65 && game_state.room_id < 72)
		game_state.area_id = 4;
	else if(game_state.room_id >= 72 && game_state.room_id < 77)
		game_state.area_id = 5;
	else if(game_state.room_id >= 77 && game_state.room_id < 83)
		game_state.area_id = 6;
	else if(game_state.room_id >= 83 && game_state.room_id < 89)
		game_state.area_id = 7;
	else
		game_state.area_id = 8;
}
