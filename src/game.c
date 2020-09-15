#include <math.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glext.h> // for mingw

#include "types.h"
#include "mtx.h"
#include "game.h"
#include "entity.h"
#include "archive.h"
#include "strings.h"
#include "hud.h"
#include "world.h"
#include "heap.h"

#ifdef _WIN32
extern PFNGLLOADTRANSPOSEMATRIXFPROC	glLoadTransposeMatrixf;
#endif

CRoom* room;
State game_state = { 0 };

Mtx44 projection;
Mtx44 view;
Mtx44 projectionview;

extern float pos_x;
extern float pos_y;
extern float pos_z;
extern float xrot;
extern float yrot;

extern bool show_entities;

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

void GAMEUnloadRoom(void)
{
	if(room) {
		CRoom_free(room);
		free_to_heap(room);
	}
	room = NULL;
}

void GAMERenderScene(float aspect)
{
	float size_x = fabsf(room->model->max_x - room->model->min_x);
	float size_y = fabsf(room->model->max_y - room->model->min_y);
	float size_z = fabsf(room->model->max_z - room->model->min_z);
	float size = size_x;
	if(size_y > size) {
		size = size_y;
	}
	if(size_z > size) {
		size = size_z;
	}

	// projection
	MTX44Perspective(&projection, 80.0f, aspect, 0.05f, 2 * size * room->model->scale);

	// view
	Mtx44 trans, rotx, roty, rot;
	MTX44Trans(&trans, -pos_x, -pos_y, -pos_z);
	MTX44RotRad(&rotx, 'x', xrot / 180.0 * M_PI);
	MTX44RotRad(&roty, 'y', (360.0f - yrot) / 180.0 * M_PI);
	MTX44Concat(&rotx, &roty, &rot);
	MTX44Concat(&rot, &trans, &view);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CModel_begin_scene();
	CRoom_render(room);
	if(show_entities)
		CEntity_render_all();
	CModel_end_scene();
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
