#include <stdio.h>
#include "types.h"
#include "strings.h"
#include "hud.h"
#include "game.h"

StringTable hud_messages_common;
StringTable hud_messages_sp;
StringTable hud_messages_mp;
StringTable location_names;
StringTable game_messages;
StringTable weaponnames;

const char* weaponnames_w[9];
const char* weaponnames_h[8];
const char* weaponnames_a[7];

void load_hud_messages()
{
	const char* folder = get_stringtable_folder();

	load_stringtable(folder, "HudMsgsCommon.bin", &hud_messages_common);
	if(game_state.game_mode == SINGLE_PLAYER) {
		load_stringtable(folder, "HudMessagesSP.bin", &hud_messages_sp);
		load_stringtable(folder, "LocationNames.bin", &location_names);
	} else {
		load_stringtable(folder, "HudMessagesMP.bin", &hud_messages_mp);
	}
}

void load_game_messages()
{
	const char* folder = get_stringtable_folder();

	// dword_20DF0E4 = 384;
	// load_message_frames_maybe();
	// load_map_quit();
	load_stringtable(folder, "GameMessages.bin", &game_messages);
}

void load_weapon_names()
{
	int i;
	char id[32];
	const char* folder = get_stringtable_folder();

	load_stringtable(folder, "WeaponNames.bin", &weaponnames);
	for(i = 0; i < 9; i++) {
		sprintf(id, "W%03d", i + 1);
		weaponnames_w[i] = get_string_by_id(&weaponnames, id);
	}
	for(i = 0; i < 8; i++) {
		sprintf(id, "H%03d", i + 1);
		weaponnames_h[i] = get_string_by_id(&weaponnames, id);
	}
	for(i = 0; i < 7; i++) {
		sprintf(id, "A%03d", i + 1);
		weaponnames_a[i] = get_string_by_id(&weaponnames, id);
	}
}

const char* get_hud_message(int id)
{
	if(id < 12)
		return get_string_by_id_and_type('H', id, &hud_messages_common);
	else if(id < 123)
		return get_string_by_id_and_type('H', id, &hud_messages_sp);
	else if(id < 258)
		return get_string_by_id_and_type('H', id, &hud_messages_mp);
	else if(id < 306)
		return get_string_by_id_and_type('W', id - 300, &hud_messages_mp);
	else
		return " ";
}
