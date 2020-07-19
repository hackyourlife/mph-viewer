#ifndef __ENTITY_H__
#define __ENTITY_H__

enum ENTITY_TYPE {
	PLATFORM = 0x0,
	OBJECT = 0x1,
	SPAWN_POINT = 0x2,
	ALIMBIC_DOOR = 0x3,
	ITEM = 0x4,
	ITEM_INSTANCE = 0x5,
	ENEMY = 0x6,
	JUMP_PAD = 0x9,
	TELEPORTER = 0xE,
	ARTIFACT = 0x11,
	CAMERA_SEQ = 0x12,
	FORCE_FIELD = 0x13,
	ENEMY_INSTANCE = 0x17,
	ENERGY_BEAM = 0x1A
};

typedef struct
{
	u16		type;
	u16		id;
} EntityData;

typedef struct {
	u8		node_name[16];
	s16		layer_mask;
	u16		length;
	EntityData*	data;
} Entity;

typedef struct {
	u32		version;
	u16		lengths[16];
} EntityFileHeader;

typedef struct {
	u8		node_name[16];
	s16		layer_mask;
	u16		length;
	u32		data;
} EntityFileEntry;

typedef struct {
	EntityData	header;

	VecFx32		pos;
	VecFx32		base_vec1;
	VecFx32		base_vec2;
	u32		field_24;
	u32		field_28;
	u32		field_2C;
	u32		field_30;
	u32		field_34;
	u32		field_38;
	u32		field_3C;
	u32		field_40;
	u32		field_44;
	u32		field_48;
	u32		field_4C;
	u32		field_50;
	u32		field_54;
	u32		field_58;
	u32		field_5C;
	u32		field_60;
	u32		field_64;
	u32		field_68;
	VecFx32		beam_vec;
	fx32		speed;
	s16		field_7C;
	u16		field_7E;
	u8		field_80;
	u8		field_81;
	s16		field_82;
	u32		model_id;
	u32		beam_type;
	u32		field_8C;
} EntityJumpPad;

typedef struct {
	EntityData	header;

	VecFx32		pos;
	u32		field_C;
	u32		field_10;
	u32		field_14;
	u32		field_18;
	u32		field_1C;
	u32		field_20;
	u32		item_id;
	u32		type;
	u8		enabled;
	u8		has_base;
	u8		field_2E;
	u8		field_2F;
	u16		max_spawn_count;
	u16		spawn_interval;
	u16		spawn_delay;
	u16		entity_id;
	u32		field_38;
	u32		field_3C;
	u32		field_40;
} EntityItem;

typedef struct {
	EntityData	header;

	VecFx32		pos;
	VecFx32		vec1;
	VecFx32		vec2;
	u32		flags;
	u32		fx_flags;
	u32		object_id;
	u16		linked_entity;
	u16		scan_id;
} EntityObject;

typedef struct {
	EntityData	header;

	VecFx32		pos;
	VecFx32		vec1;
	VecFx32		vec2;
	u8		field_24;
	u8		field_25;
	u8		artifact_id;
	u8		field_27;
	u8		invisible;
	u8		target_ent[15];
	u32		field_38;
	VecFx32		field_3C;
	u8		node_name[16];
	u32		field_58;
} EntityTeleporter;

typedef struct {
	EntityData	header;

	VecFx32		pos;
	VecFx32		vec1;
	VecFx32		vec2;

	u8		node_name[16];
	u32		palette_id;
	u32		type;
	u32		field_3C;
	u8		target_layer_id;
	u8		flags;
	u8		field_42;
	u8		field_43;
	u8		ent_filename[16];
	u8		room_name[16];
	u32		field_64;
} EntityDoor;

typedef struct {
	EntityData	header;

	VecFx32		pos;
	VecFx32		vec1;
	VecFx32		vec2;

	u32		field_24;
	u32		type;
} EntityPlatform;

struct CEntity;
typedef struct CEntity CEntity;

typedef struct {
	CEntity*	(*construct)(const char* node_name, EntityData* data);
	void		(*process)(CEntity* self, float dt);
	void		(*process_class)(float dt);
	void		(*render)(CEntity* self);
	Vec3*		(*get_position)(CEntity* self);
	void		(*set_tex_filter)(int type);
} EntityClass;

struct CEntity {
	u16		type;
	u16		scan_id;
	EntityClass*	funcs;
	CEntity*	next;
};

typedef struct {
	CEntity		base;
	EntityJumpPad*	pad;
	Vec3		pos;
	Vec3		beam_vec;
	int		model_id;
	int		beam_id;
	Mtx44		base_mtx;
	Mtx44		beam_mtx;
} CJumpPad;

typedef struct {
	CEntity		base;
	EntityItem*	item;
	Vec3		pos;
	int		model_id;
	float		rotation;
	u16		max_spawn_count;
	u16		spawn_interval;
	u16		spawn_delay;
	u8		has_base;
	u8		enabled;
} CItem;

typedef struct {
	CEntity		base;
	EntityObject*	obj;
	int		object_id;
	Vec3		pos;
	Mtx44		transform;
} CObject;

typedef struct {
	CEntity		base;
	EntityTeleporter* ent;
	Vec3		pos;
	Mtx44		transform;
	int		artifact_id;
	CModel*		model;
	u32		flags;
} CTeleporter;

typedef struct {
	CEntity		base;
	EntityDoor*	ent;
	Vec3		pos;
	Mtx44		transform;
	CModel*		model;
	int		type;
	int		palette_id;
	u32		flags;
} CAlimbicDoor;

typedef struct {
	CEntity		base;
	EntityPlatform*	ent;
	Vec3		pos;
	Mtx44		transform;
	int		type;
	u32		flags;
} CPlatform;

extern Entity* entities;

void		EntLoad(Entity** ent, const char* filename, int layer_id);
void		EntInitialize(int size);
EntityClass*	EntRegister(int id);
void		CEntityCtor(CEntity* entity, EntityData* data);

void		EntSetTextureFilter(int type);

void		CEntity_initialize(Entity* entities, NODE* node);
void		CEntity_process_all(float dt);
void		CEntity_render_all(void);

void		EntJumpPadRegister(void);
void		EntItemRegister(void);
void		EntObjectRegister(void);
void		EntTeleporterRegister(void);
void		EntAlimbicDoorRegister(void);
void		EntPlatformRegister(void);

void		get_transform_mtx(Mtx44* mtx, VecFx32* vec1, VecFx32* vec2);

#endif
