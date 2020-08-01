#ifndef __MODEL_H__
#define __MODEL_H__

#include "types.h"

typedef enum {
	LAYER_ML0	= 0x0008,
	LAYER_ML1	= 0x0010,
	LAYER_MPU	= 0x0020,
	LAYER_CTF	= 0x4000
} LAYER_MASK;

#define RELOAD_MODEL		0x02
#define USE_COMPRESSION		0x04
#define USE_ARCHIVE		0x08
#define USE_EXTERNAL_TXTR	0x40

#define	USE_LIGHT		0x1

/* animation structures */
typedef struct {
	char				material_name[32];
	int				scale_s_blend;
	int				scale_t_blend;
	int				scale_s_len;
	int				scale_t_len;
	int				scale_s_idx;
	int				scale_t_idx;
	int				rot_blend;
	int				rot_len;
	int				rot_idx;
	int				translate_s_blend;
	int				translate_t_blend;
	int				translate_s_len;
	int				translate_t_len;
	int				translate_s_idx;
	int				translate_t_idx;
} CTexcoordAnimation;

typedef struct {
	float				time;
	int				frame_count;
	int				current_frame;
	int				count;
	float*				scales;
	float*				rotations;
	float*				translations;
	CTexcoordAnimation*		animations;
} CTexcoordAnimationGroup;

typedef struct {
	char				material_name[64];
	int				diffuse_r_blend;
	int				diffuse_g_blend;
	int				diffuse_b_blend;
	int				diffuse_r_lut_len;
	int				diffuse_g_lut_len;
	int				diffuse_b_lut_len;
	int				diffuse_r_lut_idx;
	int				diffuse_g_lut_idx;
	int				diffuse_b_lut_idx;
	int				ambient_r_blend;
	int				ambient_g_blend;
	int				ambient_b_blend;
	int				ambient_r_lut_len;
	int				ambient_g_lut_len;
	int				ambient_b_lut_len;
	int				ambient_r_lut_idx;
	int				ambient_g_lut_idx;
	int				ambient_b_lut_idx;
	int				specular_r_blend;
	int				specular_g_blend;
	int				specular_b_blend;
	int				specular_r_lut_len;
	int				specular_g_lut_len;
	int				specular_b_lut_len;
	int				specular_r_lut_idx;
	int				specular_g_lut_idx;
	int				specular_b_lut_idx;
	int				alpha_blend;
	int				alpha_lut_len;
	int				alpha_lut_idx;
	int				material_id;
} CMaterialAnimation;

typedef struct {
	float				time;
	int				frame_count;
	int				current_frame;
	int				count;
	u8*				color_lut;
	CMaterialAnimation*		animations;
} CMaterialAnimationGroup;

typedef struct {
	void*				node_animations;
	CMaterialAnimationGroup**	material_animations;
	CTexcoordAnimationGroup**	texcoord_animations;
	void*				texture_animations;
	unsigned int			count;
} CAnimation;

/* model structures */
typedef struct {
	char				name[64];
	unsigned char			light;
	unsigned char			culling;
	unsigned char			x_repeat;
	unsigned char			y_repeat;
	unsigned int			polygon_mode;
	float				scale_s;
	float				scale_t;
	float				translate_s;
	float				translate_t;
	float				rot_z;
	unsigned int			alpha;
	unsigned int			texid;
	unsigned int			palid;
	unsigned int			tex;
	unsigned int			render_mode;
	Color3				diffuse;
	Color3				ambient;
	Color3				specular;
	int				texcoord_anim_id;
	int				material_anim_id;
	GXTexGen			texgen_mode;
	int				matrix_id;
	int				anim_flags;
} CMaterial;

typedef struct {
	char				name[64];
	unsigned int			parent;
	unsigned int			child;
	unsigned int			next;
	unsigned int			enabled;
	unsigned int			mesh_count;
	unsigned int			mesh_id;
	unsigned int			type;
	Vec3				scale;
	Vec3				angle;
	Vec3				pos;
	Mtx44				node_transform;
	float				offset;
	fx32				offset_raw;
} CNode;

typedef struct {
	unsigned int			matid;
	unsigned int			dlistid;
	float				bounds[3][2];
} CMesh;

typedef struct {
	unsigned int			format;
	unsigned int			width;
	unsigned int			height;
	bool				opaque;
	u8*				data;
} CTexture;

typedef struct {
	unsigned int			size;
	u16*				data;
} CPalette;

typedef struct {
	void*				scenedata;

	CMaterial*			materials;
	CNode*				nodes;
	CMesh*				meshes;
	int*				dlists;
	CTexture*			textures;
	CPalette*			palettes;
	unsigned int			num_meshes;
	unsigned int			num_textures;
	unsigned int			num_palettes;
	unsigned int			num_materials;
	unsigned int			num_dlists;
	unsigned int			num_nodes;
	float				scale;
	Vec3*				node_pos;
	Vec3*				node_initial_pos;
	Mtx44*				texture_matrices;

	bool				apply_transform;

	float				min_x;
	float				max_x;
	float				min_y;
	float				max_y;
	float				min_z;
	float				max_z;

	CAnimation*			animation;
	void*				node_animations;
	CTexcoordAnimationGroup*	texcoord_animations;
	CMaterialAnimationGroup*	material_animations;
} CModel;

int	get_node_child(const char* name, CModel* scene);
void	CModel_init(void);
void	CModel_setLights(float l1vec[4], float l1col[4], float l2vec[4], float l2col[4]);
void	CModel_setFog(bool en, float fogc[4], int fogoffset);
void	CModel_setFogDisable(bool dis);
void	load_model(CModel** model, const char* filename, int flags);
void	load_room_model(CModel** model, const char* filename, const char* txtrfilename, int flags, int layer_mask);
CModel*	CModel_load(u8* scenedata, unsigned int scenesize, u8* texturedata, unsigned int texturesize, int layer_mask);
CModel*	CModel_load_file(const char* model, const char* textures, int layer_mask);
void	CModel_set_textures(CModel* model);
void	CModel_set_texture_filter(CModel* model, int type);
void	CModel_free(CModel* scene);
void	CModel_render_all(CModel* scene, Mtx44* mtx, float alpha);
void	CModel_render_node(CModel* scene, Mtx44* mtx, int node_idx, float alpha);
void	CModel_compute_node_matrices(CModel* model, int start_idx);

void	CModel_begin_scene(void);
void	CModel_end_scene(void);

#endif
