#ifndef __ENTITY_H__
#define __ENTITY_H__

enum ENTITY_TYPE {
	PLATFORM = 0x0,
	OBJECT = 0x1,
	ALIMBIC_DOOR = 0x3,
	ITEM = 0x4,
	PICKUP = 0x6,
	JUMP_PAD = 0x9,
	TELEPORTER = 0xE,
	ARTIFACT = 0x11,
	CAMERA_SEQ = 0x12,
	FORCE_FIELD = 0x13,
	ENERGY_BEAM = 0x1A,
};

typedef struct
{
	u16		type;
	u16		some_id;
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

typedef struct
{
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

typedef struct
{
	EntityData	header;

	VecFx32		pos;
	int		field_C;
	int		field_10;
	int		field_14;
	int		field_18;
	int		field_1C;
	int		field_20;
	int		item_id;
	int		id;
	int		field_2C;
	int		field_30;
	int		field_34;
	int		field_38;
	int		field_3C;
	int		field_40;
} EntityItem;

struct CEntity;
typedef struct CEntity CEntity;

typedef struct
{
	CEntity*	(*construct)(const char* node_name, EntityData* data);
	void		(*process)(CEntity* self, float dt);
	void		(*process_class)(float dt);
	void		(*render)(CEntity* self);
	Vec3*		(*get_position)(CEntity* self);
} EntityClass;

struct CEntity {
	u16		type;
	u16		scan_id;
	EntityClass*	funcs;
	CEntity*	next;
};

typedef struct
{
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
} CItem;

extern Entity* entities;

void		EntLoad(Entity** ent, const char* filename, int layer_id);
void		EntInitialize(int size);
EntityClass*	EntRegister(int id);
void		CEntityCtor(CEntity* entity, EntityData* data);

void		CEntity_initialize(Entity* entities, NODE* node);
void		CEntity_process_all(float dt);
void		CEntity_render_all(void);

void		EntJumpPadRegister(void);
void		EntItemRegister(void);
void		EntDelayedItemRegister(void);

void		get_transform_mtx(Mtx44* mtx, VecFx32* vec1, VecFx32* vec2);

#endif
