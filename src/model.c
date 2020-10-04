#ifdef _WIN32
#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

// #define TEXDUMP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <float.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glext.h> // for mingw

#include "types.h"
#include "vec.h"
#include "mtx.h"
#include "endianess.h"
#include "model.h"
#include "animation.h"
#include "entity.h"
#include "io.h"
#include "heap.h"
#include "os.h"

#include "game.h"

#ifdef _DEBUG
#	ifdef WIN32
#		define BREAKPOINT()				{ __asm { int 3 }; }
#	else
#		define BREAKPOINT()
#	endif
#else
#	define BREAKPOINT()
#endif

void fatal(const char* message)
{
	BREAKPOINT()
	puts(message);
	exit(0);
}

#define print_once(message, ...)			{ static bool done_it = false; if(!done_it) { printf(message, __VA_ARGS__); done_it = true; } }

enum CULL_MODE {
	DOUBLE_SIDED = 0,
	BACK_SIDE = 1,
	FRONT_SIDE = 2
};

enum GXPolygonMode {
	GX_POLYGONMODE_MODULATE = 0,
	GX_POLYGONMODE_DECAL = 1,
	GX_POLYGONMODE_TOON = 2,
	GX_POLYGONMODE_SHADOW = 3
};

enum REPEAT_MODE {
	CLAMP = 0,
	REPEAT = 1,
	MIRROR = 2
};

enum RENDER_MODE {
	NORMAL = 0,
	DECAL = 1,
	TRANSLUCENT = 2
};

typedef struct {
	u8		name[64];
	u8		light;
	u8		culling;
	u8		alpha;
	u8		wireframe;
	u16		palid;
	u16		texid;
	u8		x_repeat;
	u8		y_repeat;
	Color3		diffuse;
	Color3		ambient;
	Color3		specular;
	u8		field_53;
	u32		polygon_mode;
	u8		render_mode;
	u8		anim_flags;
	u16		field_5A;
	u32		texcoord_transform_mode;
	u16		texcoord_animation_id;
	u16		field_62;
	u32		matrix_id;
	u32		scale_s;
	u32		scale_t;
	u16		rot_z;
	u16		field_72;
	u32		translate_s;
	u32		translate_t;
	u16		material_animation_id;
	u16		field_7E;
	u8		packed_repeat_mode;
	u8		field_81;
	u16		field_82;
} Material;

typedef struct {
	u32		start_ofs;
	u32		size;
	s32		bounds[2][3];
} Dlist;

typedef struct {
	u8		name[64];
	u16		parent;
	u16		child;
	u16		next;
	u16		field_46;
	u32		enabled;
	u16		mesh_count;
	u16		mesh_id;
	VecFx32		scale;
	fx16		angle_x;
	fx16		angle_y;
	fx16		angle_z;
	u16		field_62;
	VecFx32		pos;
	fx32		offset;
	VecFx32		vec1;
	VecFx32		vec2;
	u8		type;
	u8		field_8D;
	u16		field_8E;
	MtxFx43		node_transform;
	u32		field_C0;
	u32		field_C4;
	u32		field_C8;
	u32		field_CC;
	u32		field_D0;
	u32		field_D4;
	u32		field_D8;
	u32		field_DC;
	u32		field_E0;
	u32		field_E4;
	u32		field_E8;
	u32		field_EC;
} Node;

typedef struct {
	u16		matid;
	u16		dlistid;
} Mesh;

typedef struct {
	u16		format;
	u16		width;
	u16		height;
	u16		pad;
	u32		image_ofs;
	u32		imagesize;
	u32		dunno1;
	u32		dunno2;
	u32		vram_offset;
	u32		opaque;
	u32		some_value;
	u8		packed_size;
	u8		native_texture_format;
	u16		texture_obj_ref;
} Texture;

typedef struct {
	u32		entries_ofs;
	u32		count;
	u32		dunno1;
	u32		some_reference;
} Palette;

typedef struct {
	u32		modelview_mtx_shamt;
	s32		scale;
	u32		unk3;
	u32		unk4;
	u32		materials;
	u32		dlists;
	u32		nodes;
	u16		num_node_weight;
	u8		flags;
	u8		field_1F;
	u32		node_weights;
	u32		meshes;
	u16		num_textures;
	u16		field_2A;
	u32		textures;
	u16		num_palettes;
	u16		field_32;
	u32		palettes;
	u32		some_anim_counts;
	u32		unk8;
	u32		node_initial_pos;
	u32		node_pos;
	u16		num_materials;
	u16		num_nodes;
	u32		texture_matrices;
	u32		node_animations;
	u32		texcoord_animations;
	u32		material_animations;
	u32		texture_animations;
	u16		num_meshes;
	u16		num_texture_matrices;
} __attribute__((__packed__)) HEADER;

#ifdef _WIN32
PFNGLCREATESHADERPROC		glCreateShader;	
PFNGLSHADERSOURCEPROC		glShaderSource;
PFNGLCOMPILESHADERPROC		glCompileShader;
PFNGLGETSHADERIVPROC		glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC	glGetShaderInfoLog;
PFNGLDELETESHADERPROC		glDeleteShader;
PFNGLCREATEPROGRAMPROC		glCreateProgram;
PFNGLATTACHSHADERPROC		glAttachShader;
PFNGLDETACHSHADERPROC		glDetachShader;
PFNGLLINKPROGRAMPROC		glLinkProgram;
PFNGLGETPROGRAMIVPROC		glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC	glGetProgramInfoLog;
PFNGLDELETEPROGRAMPROC		glDeleteProgram;
PFNGLUSEPROGRAMPROC		glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC	glGetUniformLocation;
PFNGLUNIFORM1IPROC		glUniform1i;
PFNGLUNIFORM1FPROC		glUniform1f;
PFNGLUNIFORM3FVPROC		glUniform3fv;
PFNGLUNIFORM4FVPROC		glUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC	glUniformMatrix4fv;
PFNGLLOADTRANSPOSEMATRIXFPROC	glLoadTransposeMatrixf;
PFNGLMULTTRANSPOSEMATRIXFPROC	glMultTransposeMatrixf;

static void load_extensions(void)
{
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC)wglGetProcAddress("glLoadTransposeMatrixf");
	glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC)wglGetProcAddress("glMultTransposeMatrixf");
}
#endif

const char* vertex_shader = "\
#version 120 \n\
uniform bool use_light; \n\
uniform vec3 light1vec; \n\
uniform vec3 light1col; \n\
uniform vec3 light2vec; \n\
uniform vec3 light2col; \n\
uniform vec3 diffuse; \n\
uniform vec3 ambient; \n\
uniform vec3 specular; \n\
uniform mat4 projection; \n\
uniform mat4 view; \n\
uniform mat4 texcoordmtx; \n\
uniform mat4[32] mtx_stack; \n\
\n\
varying vec2 texcoord; \n\
varying vec4 color; \n\
\n\
vec3 light_calc(vec3 light_vec, vec3 light_col, vec3 normal_vec, vec3 dif_col, vec3 amb_col, vec3 spe_col) \n\
{ \n\
	vec3 sight_vec = vec3(0.0, 0.0, -1.0); \n\
	float dif_factor = max(0.0, -dot(light_vec, normal_vec)); \n\
	vec3 half_vec = (light_vec + sight_vec) / 2.0; \n\
	float spe_factor = max(0.0, dot(-half_vec, normal_vec)); \n\
	spe_factor = spe_factor * spe_factor; \n\
	vec3 spe_out = spe_col * light_col * spe_factor; \n\
	vec3 dif_out = dif_col * light_col * dif_factor; \n\
	vec3 amb_out = amb_col * light_col; \n\
	return spe_out + dif_out + amb_out; \n\
} \n\
\n\
void main() \n\
{ \n\
	mat4 model = mtx_stack[int(gl_MultiTexCoord0.z)]; \n\
	gl_Position = projection * view * model * gl_Vertex; \n\
	if(use_light) { \n\
		vec3 normal = normalize(mat3(model) * gl_Normal); \n\
		vec3 dif = gl_Color.a < 0.5 ? gl_Color.rgb : diffuse; \n\
		vec3 amb = gl_Color.a < 0.5 ? vec3(0.0, 0.0, 0.0) : ambient; \n\
		vec3 col1 = light_calc(light1vec, light1col, normal, dif, amb, specular); \n\
		vec3 col2 = light_calc(light2vec, light2col, normal, dif, amb, specular); \n\
		color = vec4(min((col1 + col2), vec3(1.0, 1.0, 1.0)), 1.0); \n\
	} else { \n\
		color = vec4(gl_Color.rgb, 1.0); \n\
	} \n\
	texcoord = vec2(texcoordmtx * vec4(gl_MultiTexCoord0.xy, 0, 1)); \n\
}";
const char* fragment_shader = "\
#version 120 \n\
uniform bool use_texture; \n\
uniform bool fog_enable; \n\
uniform vec4 fog_color; \n\
uniform float fog_min; \n\
uniform float fog_max; \n\
uniform float alpha_scale; \n\
uniform sampler2D tex; \n\
varying vec2 texcoord; \n\
varying vec4 color; \n\
uniform float mat_alpha; \n\
uniform int mat_mode; \n\
uniform vec3[32] toon_table; \n\
\n\
vec4 toon_color(vec4 vtx_color) \n\
{ \n\
	return vec4(toon_table[int(vtx_color.r * 31)], vtx_color.a); \n\
} \n\
\n\
void main() \n\
{ \n\
	vec4 col; \n\
	if(use_texture) { \n\
		vec4 texcolor = texture2D(tex, texcoord); \n\
		if (mat_mode == 1) { \n\
			col = vec4( \n\
				(texcolor.r * texcolor.a + color.r * (1 - texcolor.a)), \n\
				(texcolor.g * texcolor.a + color.g * (1 - texcolor.a)), \n\
				(texcolor.b * texcolor.a + color.b * (1 - texcolor.a)), \n\
				mat_alpha * color.a \n\
			); \n\
		} else if (mat_mode == 2) { \n\
			vec4 toon = toon_color(color); \n\
			col = vec4(texcolor.rgb * toon.rgb + toon.rgb, mat_alpha * texcolor.a * color.a); \n\
		} else { \n\
			col = color * vec4(texcolor.rgb, mat_alpha * texcolor.a); \n\
		} \n\
	} else { \n\
		col = mat_mode == 2 ? toon_color(color) : color; \n\
		col.a *= mat_alpha; \n\
	} \n\
	if(fog_enable) { \n\
		float depth = gl_FragCoord.z; \n\
		float density = 0.0; \n\
		if (depth >= fog_max) { \n\
			density = 1.0; \n\
		} else if (depth > fog_min) { \n\
			// MPH fog table has min 0 and max 124 \n\
			density = (depth - fog_min) / (fog_max - fog_min) * 124.0 / 128.0; \n\
		} \n\
		gl_FragColor = vec4((col * (1.0 - density) + fog_color * density).xyz, col.a * alpha_scale); \n\
	} else { \n\
		gl_FragColor = col * vec4(1.0, 1.0, 1.0, alpha_scale); \n\
	} \n\
}";

static GLuint shader;
static GLuint use_light;
static GLuint use_texture;
static GLuint fog_enable;
static GLuint light1vec;
static GLuint light2vec;
static GLuint light1col;
static GLuint light2col;
static GLuint diffuse;
static GLuint ambient;
static GLuint specular;
static GLuint fog_color;
static GLuint fog_min;
static GLuint fog_max;
static GLuint alpha_scale;
static GLuint proj_matrix;
static GLuint view_matrix;
static GLuint matrix_stack;
static GLuint texcoord_matrix;
static GLuint mat_alpha;
static GLuint mat_mode;
static GLuint toon_table;

static float l1v[3];
static float l1c[3];
static float l2v[3];
static float l2c[3];

static float l1v_override[3];
static float l1c_override[3];
static float l2v_override[3];
static float l2c_override[3];

static bool fogen = false;
static bool fogdis = false;
static float fogcol[4];
static float fogmin;
static float fogmax;

void CModel_setLights(float l1vec[3], float l1col[3], float l2vec[3], float l2col[3])
{
	memcpy(l1v, l1vec, sizeof(float[3]));
	memcpy(l1c, l1col, sizeof(float[3]));
	memcpy(l2v, l2vec, sizeof(float[3]));
	memcpy(l2c, l2col, sizeof(float[3]));
}

void CModel_setFog(bool en, float fogc[4], int fogoffset, int fogslope)
{
	fogen = en;
	memcpy(fogcol, fogc, sizeof(float[4]));
	fogmin = fogoffset / (float)0x7FFF;
	fogmax = (fogoffset + 32 * (0x400 >> fogslope)) / (float)0x7FFF;
}

void CModel_setFogDisable(bool dis)
{
	fogdis = dis;
}

void CModel_init(void)
{
#ifdef _WIN32
	load_extensions();
#endif

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, 0);
	glCompileShader(vs);

	GLint compiled = 0;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
	if(compiled == GL_FALSE) {
		GLint len = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len);

		if(!len)
			fatal("Failed to retrieve shader compilation error log");

		char* log = (char*)malloc(len);
		glGetShaderInfoLog(vs, len, &len, log);
		glDeleteShader(vs);

		fatal(log);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, 0);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
	if(compiled == GL_FALSE) {
		GLint len = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &len);

		if(!len)
			fatal("Failed to retrieve shader compilation error log");

		char* log = (char*)malloc(len);
		glGetShaderInfoLog(fs, len, &len, log);
		glDeleteShader(fs);

		fatal(log);
	}

	shader = glCreateProgram();

	glAttachShader(shader, vs);
	glAttachShader(shader, fs);
	glLinkProgram(shader);

	GLint linked = 0;
	glGetProgramiv(shader, GL_LINK_STATUS, (int*)&linked);
	if(linked == GL_FALSE) {
		GLint len = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &len);

		char* log = (char*)malloc(len);
		glGetProgramInfoLog(shader, len, &len, log);

		glDeleteProgram(shader);
		glDeleteShader(vs);
		glDeleteShader(fs);

		fatal(log);
	}

	glDetachShader(shader, vs);
	glDetachShader(shader, fs);

	use_light = glGetUniformLocation(shader, "use_light");
	use_texture = glGetUniformLocation(shader, "use_texture");
	fog_enable = glGetUniformLocation(shader, "fog_enable");
	light1vec = glGetUniformLocation(shader, "light1vec");
	light1col = glGetUniformLocation(shader, "light1col");
	light2vec = glGetUniformLocation(shader, "light2vec");
	light2col = glGetUniformLocation(shader, "light2col");
	diffuse = glGetUniformLocation(shader, "diffuse");
	ambient = glGetUniformLocation(shader, "ambient");
	specular = glGetUniformLocation(shader, "specular");
	fog_color = glGetUniformLocation(shader, "fog_color");
	fog_min = glGetUniformLocation(shader, "fog_min");
	fog_max = glGetUniformLocation(shader, "fog_max");
	alpha_scale = glGetUniformLocation(shader, "alpha_scale");
	proj_matrix = glGetUniformLocation(shader, "projection");
	view_matrix = glGetUniformLocation(shader, "view");
	matrix_stack = glGetUniformLocation(shader, "mtx_stack");
	texcoord_matrix = glGetUniformLocation(shader, "texcoordmtx");
	mat_alpha = glGetUniformLocation(shader, "mat_alpha");
	mat_mode = glGetUniformLocation(shader, "mat_mode");
	toon_table = glGetUniformLocation(shader, "toon_table");
}

unsigned int crc32(u8* data, u32 len) {
	int i, j;
	unsigned int byte, crc, mask;

	i = 0;
	crc = 0xFFFFFFFF;
	while(len--) {
		crc = crc ^ data[i];
		for (j = 7; j >= 0; j--) {
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
		i++;
	}
	return ~crc;
}

#ifdef TEXDUMP
typedef struct {
	u16	bfType;
	u32	bfSize;
	u32	bfReserved;
	u32	bfOffBits;
	u32	biSize;
	u32	biWidth;
	u32	biHeight;
	u16	biPlanes;
	u16	biBitCount;
	u32	biCompression;
	u32	biSizeImage;
	u32	biXPelsPerMeter;
	u32	biYPelsPerMeter;
	u32	biClrUsed;
	u32	biClrImportant;
} __attribute__((packed, aligned(1))) BMP;
#endif

typedef struct {
	char	name[64];
	u32	checksum;
	int	x_repeat;
	int	y_repeat;
	int	filter;
} TEXOVERRIDE;

#define	NUM_OVERRIDES	13

static TEXOVERRIDE mtl_overrides[NUM_OVERRIDES] = {
	/* alimbic door glow */
	{ "AlimbicDoorGlow_Material", 0x84C7B8C1, -1, MIRROR, 1 },
	/* lava terminal */
	{ "Glow_Material", 0x6C64B284, -1, CLAMP, -1 },
	/* alimbic terminal */
	{ "Text_Material", 0xE79BB5B0, -1, -1, 0 },
	/* proximity force fields */
	{ "lambert179", 0xC3CB70DB, -1, -1, 1 },
	{ "pmag1", 0x7E029EA2, -1, -1, 1 },
	{ "lambert161", 0xBBB3556B, -1, -1, 1 },
	/* tetra galaxy map */
	{ "lambert44", 0x3B098D90, -1, -1, 0 },
	/* pole swirl */
	{ "swirl", 0x7680D648, -1, -1, 1 },
	/* light rays */
	{ "LightGrad", 0x9A4B2EBB, CLAMP, CLAMP, 1 },
	/* stronghold void */
	{ "fire_pit", 0xF25B969A, -1, -1, 1 },
	{ "roof", 0xEC5CFB0F, -1, -1, 1 },
	/* alimbic display */
	{ "lambert4", 0xF754F43F, -1, -1, 0 },
	/* incubation vault window */
	{ "lambert33", 0x7AE0614E, -1, MIRROR, 1 }
};

static void update_bounds(CModel* scene, float vtx_state[3])
{
	if(vtx_state[0] < scene->min_x) {
		scene->min_x = vtx_state[0];
	} else if(vtx_state[0] > scene->max_x) {
		scene->max_x = vtx_state[0];
	}
	if(vtx_state[1] < scene->min_y) {
		scene->min_y = vtx_state[1];
	} else if(vtx_state[1] > scene->max_y) {
		scene->max_y = vtx_state[1];
	}
	if(vtx_state[2] < scene->min_z) {
		scene->min_z = vtx_state[0];
	} else if(vtx_state[2] > scene->max_z) {
		scene->max_z = vtx_state[2];
	}
}

static void do_reg(u32 reg, u32** data_pp, float vtx_state[3], float uv_state[2], unsigned int* mtx_id, CModel* scene)
{
	u32* data = *data_pp;

	switch(reg) {
		//NOP
		case 0x400: {
		}
		break;

		//MTX_RESTORE
		case 0x450: {
			u32 index = *(data++);
			// e.g. rooms don't use weight IDs, so keep the index at 0
			if (scene->num_node_weight > 0) {
				*mtx_id = index;
			}
			glTexCoord3f(uv_state[0], uv_state[1], (float)(*mtx_id));
		}
		break;

		//COLOR
		case 0x480: {
			u32 rgb = *(data++);
			u32 r = (rgb >>  0) & 0x1F;
			u32 g = (rgb >>  5) & 0x1F;
			u32 b = (rgb >> 10) & 0x1F;
			glColor3f(((float)r) / 31.0f, ((float)g) / 31.0f, ((float)b) / 31.0f);
		}
		break;

		//NORMAL
		case 0x484: {
			u32 xyz = *(data++);
			s32 x = (xyz >>  0) & 0x3FF;			if(x & 0x200)			x |= 0xFFFFFC00;
			s32 y = (xyz >> 10) & 0x3FF;			if(y & 0x200)			y |= 0xFFFFFC00;
			s32 z = (xyz >> 20) & 0x3FF;			if(z & 0x200)			z |= 0xFFFFFC00;
			glNormal3f(((float)x) / 512.0f, ((float)y) / 512.0f, ((float)z) / 512.0f);
		}
		break;

		//TEXCOORD
		case 0x488: {
			u32 st = *(data++);
			s32 s = (st >>  0) & 0xFFFF;			if(s & 0x8000)		s |= 0xFFFF0000;
			s32 t = (st >> 16) & 0xFFFF;			if(t & 0x8000)		t |= 0xFFFF0000;
			uv_state[0] = ((float)s) / 16.0f;
			uv_state[1] = ((float)t) / 16.0f;
			glTexCoord3f(uv_state[0], uv_state[1], (float)(*mtx_id));
		}
		break;

		//VTX_16
		case 0x48C: {
			u32 xy = *(data++);
			s32 x = (xy >>  0) & 0xFFFF;			if(x & 0x8000)		x |= 0xFFFF0000;
			s32 y = (xy >> 16) & 0xFFFF;			if(y & 0x8000)		y |= 0xFFFF0000;
			s32 z = (*(data++)) & 0xFFFF;			if(z & 0x8000)		z |= 0xFFFF0000;

			vtx_state[0] = ((float)x) / 4096.0f;
			vtx_state[1] = ((float)y) / 4096.0f;
			vtx_state[2] = ((float)z) / 4096.0f;
			glVertex3fv(vtx_state);

			update_bounds(scene, vtx_state);
		}
		break;

		//VTX_10
		case 0x490: {
			u32 xyz = *(data++);
			s32 x = (xyz >>  0) & 0x3FF;			if(x & 0x200)		x |= 0xFFFFFC00;
			s32 y = (xyz >> 10) & 0x3FF;			if(y & 0x200)		y |= 0xFFFFFC00;
			s32 z = (xyz >> 20) & 0x3FF;			if(z & 0x200)		z |= 0xFFFFFC00;
			vtx_state[0] = ((float)x) / 64.0f;
			vtx_state[1] = ((float)y) / 64.0f;
			vtx_state[2] = ((float)z) / 64.0f;
			glVertex3fv(vtx_state);

			update_bounds(scene, vtx_state);
		}
		break;

		//VTX_XY
		case 0x494: {
			u32 xy = *(data++);
			s32 x = (xy >>  0) & 0xFFFF;			if(x & 0x8000)		x |= 0xFFFF0000;
			s32 y = (xy >> 16) & 0xFFFF;			if(y & 0x8000)		y |= 0xFFFF0000;
			vtx_state[0] = ((float)x) / 4096.0f;
			vtx_state[1] = ((float)y) / 4096.0f;
			glVertex3fv(vtx_state);

			update_bounds(scene, vtx_state);
		}
		break;

		//VTX_XZ
		case 0x498: {
			u32 xz = *(data++);
			s32 x = (xz >>  0) & 0xFFFF;			if(x & 0x8000)		x |= 0xFFFF0000;
			s32 z = (xz >> 16) & 0xFFFF;			if(z & 0x8000)		z |= 0xFFFF0000;
			vtx_state[0] = ((float)x) / 4096.0f;
			vtx_state[2] = ((float)z) / 4096.0f;
			glVertex3fv(vtx_state);

			update_bounds(scene, vtx_state);
		}
		break;

		//VTX_YZ
		case 0x49C: {
			u32 yz = *(data++);
			s32 y = (yz >>  0) & 0xFFFF;			if(y & 0x8000)		y |= 0xFFFF0000;
			s32 z = (yz >> 16) & 0xFFFF;			if(z & 0x8000)		z |= 0xFFFF0000;
			vtx_state[1] = ((float)y) / 4096.0f;
			vtx_state[2] = ((float)z) / 4096.0f;
			glVertex3fv(vtx_state);

			update_bounds(scene, vtx_state);
		}
		break;

		//VTX_DIFF
		case 0x4A0: {
			u32 xyz = *(data++);
			s32 x = (xyz >>  0) & 0x3FF;			if(x & 0x200)		x |= 0xFFFFFC00;
			s32 y = (xyz >> 10) & 0x3FF;			if(y & 0x200)		y |= 0xFFFFFC00;
			s32 z = (xyz >> 20) & 0x3FF;			if(z & 0x200)		z |= 0xFFFFFC00;
			vtx_state[0] += ((float)x) / 4096.0f;
			vtx_state[1] += ((float)y) / 4096.0f;
			vtx_state[2] += ((float)z) / 4096.0f;
			glVertex3fv(vtx_state);

			update_bounds(scene, vtx_state);
		}
		break;

		//DIF_AMB
		case 0x4C0: {
			u32 rgb = *(data++);
			u32 r = (rgb >>  0) & 0x1F;
			u32 g = (rgb >>  5) & 0x1F;
			u32 b = (rgb >> 10) & 0x1F;
			glColor4f(((float)r) / 31.0f, ((float)g) / 31.0f, ((float)b) / 31.0f, 0);
		}
		break;

		//BEGIN_VTXS
		case 0x500: {
			u32 type = *(data++);
			switch( type )
			{
				case 0:		glBegin(GL_TRIANGLES);			break;
				case 1:		glBegin(GL_QUADS);			break;
				case 2:		glBegin(GL_TRIANGLE_STRIP);		break;
				case 3:		glBegin(GL_QUAD_STRIP);			break;
				default:	fatal("Bogus geom type\n");		break;
			}
		}
		break;

		//END_VTXS
		case 0x504: {
			glEnd();
		}
		break;

		//Hmm?
		default: {
			printf("unhandled write to register 0x%x\n", 0x4000000 + reg);
			fatal("Unhandled reg write\n");
		}
		break;
	}

	*data_pp = data;
}


/*

	First word is 4 byte-sized register indexes (where index is quadrupled and added to 0x04000400 to yield an address)
	These are processed least significant byte first
	Register index 0 means NOP
	Following this is the data to be written to each register, which is variable depending on the register being written

*/

static void do_dlist(u32* data, u32 len, CModel* scene)
{
	u32* end = data + len / 4;

	float vtx_state[3] = { 0.0f, 0.0f, 0.0f };
	float uv_state[2] = { 0.0f, 0.0f };
	unsigned int mtx_id = 0;

	glTexCoord3f(0.0f, 0.0f, 0.0f);
	while(data < end) {
		u32 regs = *(data++);

		u32 c;
		for(c = 0; c < 4; c++,regs >>= 8) {
			u32 reg = ((regs & 0xFF) << 2) + 0x400;

			do_reg(reg, &data, vtx_state, uv_state, &mtx_id, scene);
		}
	}
	glTexCoord3f(0.0f, 0.0f, 0.0f);
}

static void build_meshes(CModel* scene, Mesh* meshes, Dlist* dlists, unsigned int mesh_count, void* scenedata)
{
	scene->meshes = (CMesh*) malloc(scene->num_meshes * sizeof(CMesh));
	if(!scene->meshes)
		fatal("not enough memory");

	unsigned int i;
	Mesh* mesh;
	CMesh* m;

	scene->min_x = FLT_MAX;
	scene->min_y = FLT_MAX;
	scene->min_z = FLT_MAX;
	scene->max_x = -FLT_MAX;
	scene->max_y = -FLT_MAX;
	scene->max_z = -FLT_MAX;

	for(i = 0, mesh = meshes, m = scene->meshes; i < mesh_count; mesh++, m++, i++) {
		Dlist* dlist = &dlists[mesh->dlistid];
		u32* data = (u32*) (scenedata + dlist->start_ofs);
		m->matid = mesh->matid;
		m->dlistid = mesh->dlistid;
		if(scene->dlists[m->dlistid] != -1)
			continue;
		scene->dlists[m->dlistid] = glGenLists(1);
		glNewList(scene->dlists[m->dlistid], GL_COMPILE);
		do_dlist(data, dlist->size, scene);
		glEndList();
	}
}

/*

	Texture formats:

		0 = 2bit palettised
		1 = 4bit palettised
		2 = 8bit palettised
		4 = 8bit greyscale
		5 = 16bit RGBA

	There may be more, but these are the only ones used by Metroid

	Palette entries are 16bit RGBA

*/
static void make_textures(CModel* model)
{
	u32 m;
	for(m = 0; m < model->num_materials; m++) {
		CMaterial* mat = &model->materials[m];
		if(mat->texid == 0xFFFF)
			continue;
		if(mat->texid >= model->num_textures) {
			printf("invalid texture id %04X for material %d\n", mat->texid, m);
			continue;
		}

		if(mat->palid != 0xFFFF && !model->palettes)
			fatal("missing palette");

		CTexture* tex = &model->textures[mat->texid];
		CPalette* pal = &model->palettes[mat->palid];

		u8* texels = tex->data;
		u16* paxels = pal->data;

		u32 num_pixels = (u32)tex->width * (u32)tex->height;

		u32* image = (u32*) malloc(sizeof(u32) * num_pixels);
		if(!image)
			fatal("not enough memory");

		float alpha = mat->alpha / 31.0;

		if(tex->format == 0) {				// 2bit palettised
			u32 p;
			for(p = 0; p < num_pixels; p++) {
				u32 index = texels[p / 4];
				index = (index >> ((p % 4) * 2)) & 0x3;
				u16 col = get16bit_LE((u8*)&paxels[index]);
				u32 r = ((col >>  0) & 0x1F) << 3;
				u32 g = ((col >>  5) & 0x1F) << 3;
				u32 b = ((col >> 10) & 0x1F) << 3;
				u32 a = (tex->opaque ? 0xFF : (index == 0 ? 0x00 : 0xFF)) * alpha;
				image[p] = (r << 0) | (g << 8) | (b << 16) | (a << 24);
			}
		} else if(tex->format == 1) {			// 4bit palettised
			u32 p;
			for(p = 0; p < num_pixels; p++) {
				u32 index = texels[p / 2];
				index = (index >> ((p % 2) * 4)) & 0xF;
				u16 col = get16bit_LE((u8*)&paxels[index]);
				u32 r = ((col >>  0) & 0x1F) << 3;
				u32 g = ((col >>  5) & 0x1F) << 3;
				u32 b = ((col >> 10) & 0x1F) << 3;
				u32 a = (tex->opaque ? 0xFF : (index == 0 ? 0x00 : 0xFF)) * alpha;
				image[p] = (r << 0) | (g << 8) | (b << 16) | (a << 24);
			}
		} else if(tex->format == 2) {			// 8bit palettised
			u32 p;
			for(p = 0; p < num_pixels; p++) {
				u32 index = texels[p];
				u16 col = get16bit_LE((u8*)&paxels[index]);
				u32 r = ((col >>  0) & 0x1F) << 3;
				u32 g = ((col >>  5) & 0x1F) << 3;
				u32 b = ((col >> 10) & 0x1F) << 3;
				u32 a = (tex->opaque ? 0xFF : (index == 0 ? 0x00 : 0xFF)) * alpha;
				image[p] = (r << 0) | (g << 8) | (b << 16) | (a << 24);
			}
		} else if(tex->format == 4) {			// A5I3
			u32 p;
			for(p = 0; p < num_pixels; p++) {
				u8 entry = texels[p];
				u8 i = (entry & 0x07);
				u16 col = get16bit_LE((u8*)&paxels[i]);
				u32 r = ((col >>  0) & 0x1F) << 3;
				u32 g = ((col >>  5) & 0x1F) << 3;
				u32 b = ((col >> 10) & 0x1F) << 3;
				u32 a = (tex->opaque ? 0xFF : ((entry >> 3) / 31.0 * 255.0)) * alpha;
				image[p] = (r << 0) | (g << 8) | (b << 16) | (a << 24);
			}
		} else if(tex->format == 5) {			// 16it RGB
			u32 p;
			for(p = 0; p < num_pixels; p++) {
				u16 col = (u16)texels[p * 2 + 0] | (((u16)texels[p * 2 + 1]) << 8);
				u32 r = ((col >> 0) & 0x1F) << 3;
				u32 g = ((col >> 5) & 0x1F) << 3;
				u32 b = ((col >> 10) & 0x1F) << 3;
				u32 a = (tex->opaque ? 0xFF : ((col & 0x8000) ? 0xFF : 0x00)) * alpha;
				image[p] = (r << 0) | (g << 8) | (b << 16) | (a << 24);
			}
		} else if(tex->format == 6) {			// A3I5
			u32 p;
			for(p = 0; p < num_pixels; p++) {
				u8 entry = texels[p];
				u32 i = entry & 0x1F;
				u16 col = get16bit_LE((u8*)&paxels[i]);
				u32 r = ((col >>  0) & 0x1F) << 3;
				u32 g = ((col >>  5) & 0x1F) << 3;
				u32 b = ((col >> 10) & 0x1F) << 3;
				u32 a = (tex->opaque ? 0xFF : ((entry >> 5) / 7.0 * 255.0)) * alpha;
				image[p] = (r << 0) | (g << 8) | (b << 16) | (a << 24);
			}
		} else {
			BREAKPOINT();
			print_once("Unhandled texture-format: %d\n", tex->format);
			memset(image, 0x7F, num_pixels * 4);
		}

		u32 p;
		switch(mat->polygon_mode) {
			case GX_POLYGONMODE_MODULATE:
				break;
			case GX_POLYGONMODE_DECAL:
				for(p = 0; p < num_pixels; p++) {
					u32 col = image[p];
					u32 r = (col >>  0) & 0xFF;
					u32 g = (col >>  8) & 0xFF;
					u32 b = (col >> 16) & 0xFF;
					u32 a = 0xFF * alpha;
					image[p] = (r << 0) | (g << 8) | (b << 16) | (a << 24);
				}
				break;
			case GX_POLYGONMODE_TOON:
				break;
			case GX_POLYGONMODE_SHADOW:
				break;
			default:
				printf("unknown alpha mode %d\n", mat->alpha);
				break;
		}

		int translucent = 0;
		for(p = 0; p < num_pixels; p++) {
			u32 col = image[p];
			u32 a = (col >> 24) & 0xFF;
			if(a != 0xFF) {
				translucent = 1;
				break;
			}
		}
		if(mat->alpha < 31) {
			mat->render_mode = TRANSLUCENT;
			translucent = 1;
		}
		if(mat->render_mode != NORMAL) {
			if(!translucent) {
				printf("%d [%s]: strange, this should be opaque (alpha: %d, fmt: %d, opaque: %d)\n", m, mat->name, mat->alpha, tex->format, tex->opaque);
				mat->render_mode = NORMAL;
			}
		} else if(translucent) {
			// there are translucent pixels, but the material is not marked as translucent
			printf("%d [%s]: strange, this should be translucent (alpha: %d, fmt: %d, opaque: %d)\n", m, mat->name, mat->alpha, tex->format, tex->opaque);
			mat->render_mode = TRANSLUCENT;
		}

		glGenTextures(1, &mat->tex);
		glBindTexture(GL_TEXTURE_2D, mat->tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image);
#if 0
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
		switch(mat->x_repeat) {
			case CLAMP:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				break;
			case REPEAT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				break;
			case MIRROR:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				break;
			default:
				printf("unknown repeat mode %d\n", mat->x_repeat);
		}
		switch(mat->y_repeat) {
			case CLAMP:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case REPEAT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			case MIRROR:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
			default:
				printf("unknown repeat mode %d\n", mat->x_repeat);
		}

		u32 texsize = num_pixels;
		u32 palsize = 0;
		switch(tex->format) {
			case 0:
				texsize /= 4;
				palsize = 8;
				break;
			case 1:
				texsize /= 2;
				palsize = 32;
				break;
			case 2:
				palsize = 512;
				break;
			case 4:
				palsize = 16;
				break;
			case 5:
				texsize *= 2;
				break;
			case 6:
				palsize = 64;
		}

		u32 hash = crc32(texels, texsize);
		u32 palhash = crc32((u8*) paxels, pal->size);

#ifdef TEXDUMP
		char filename[128];
		if(mat->name[0] == 0) {
			sprintf(filename, "texdump/%dx%d-%08x-%08x.bmp", tex->width, tex->height, hash, palhash);
		} else {
			sprintf(filename, "texdump/%dx%d-%08x-%08x-%s.bmp", tex->width, tex->height, hash, palhash, mat->name);
		}
		FILE* f = fopen(filename, "wb");
		BMP bmp;
		bmp.bfType = 0x4D42;
		bmp.bfSize = sizeof(BMP) + tex->width * tex->height * 4;
		bmp.bfReserved = 0;
		bmp.bfOffBits = sizeof(BMP);
		bmp.biSize = 40;
		bmp.biWidth = tex->width;
		bmp.biHeight = tex->height;
		bmp.biPlanes = 1;
		bmp.biBitCount = 32;
		bmp.biCompression = 0;
		bmp.biSizeImage = 0;
		bmp.biXPelsPerMeter = 0;
		bmp.biYPelsPerMeter = 0;
		bmp.biClrUsed = 0;
		bmp.biClrImportant = 0;
		fwrite(&bmp, sizeof(BMP), 1, f);
		for(u32 n = 0; n < (tex->width * tex->height); n++) {
			u32 px = image[n];
			u32 r = (px >>  0) & 0xFF;
			u32 g = (px >>  8) & 0xFF;
			u32 b = (px >> 16) & 0xFF;
			u32 a = (px >> 24) & 0xFF;
			px = b | (g << 8) | (r << 16) | (a << 24);
			fwrite(&px, 4, 1, f);
		}
		fclose(f);
#endif

		for(u32 n = 0; n < NUM_OVERRIDES; n++) {
			TEXOVERRIDE* ovr = &mtl_overrides[n];
			if(ovr->checksum == hash) {
				if(ovr->x_repeat >= 0) {
					switch(ovr->x_repeat) {
						case CLAMP:
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
							break;
						case REPEAT:
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
							break;
						case MIRROR:
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
							break;
						default:
							printf("unknown repeat mode %d\n", ovr->x_repeat);
					}
				}
				if(ovr->y_repeat >= 0) {
					switch(ovr->y_repeat) {
						case CLAMP:
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
							break;
						case REPEAT:
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
							break;
						case MIRROR:
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
							break;
						default:
							printf("unknown repeat mode %d\n", ovr->y_repeat);
					}
				}
				if(ovr->filter >= 0) {
					int type = ovr->filter ? GL_LINEAR : GL_NEAREST;
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);
				}
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		free(image);
	}
}

static char* get_room_node_name(CNode* nodes, unsigned int node_cnt)
{
	char* name = "rmMain";
	if(node_cnt > 0) {
		CNode* node = nodes;
		int i = 0;
		while(node->name[0] != 'r' || node->name[1] != 'm') {
			node++;
			i++;
			if(i >= node_cnt) {
				return name;
			}
		}
		name = node->name;
	}
	return name;
}

int get_node_child(const char* name, CModel* scene)
{
	unsigned int i;
	if(scene->num_nodes <= 0)
		return -1;
	for(i = 0; i < scene->num_nodes; i++) {
		CNode* node = &scene->nodes[i];
		if(!strcmp(node->name, name))
			return node->child;
	}
	return -1;
}

void CModel_filter_nodes(CModel* scene, int layer_mask)
{
	unsigned int i;
	for(i = 0; i < scene->num_nodes; i++) {
		CNode* node = &scene->nodes[i];
		int flags = 0;
		if(strlen(node->name)) {
			unsigned int p;
			int keep = 0;
			for(p = 0; p < strlen(node->name); p += 4) {
				char* ch1 = &node->name[p];
				if(*ch1 != '_')
					break;
				if(*(u16*)ch1 == *(u16*)"_s") {
					int nr = node->name[p + 3] - '0' + 10 * (node->name[p + 2] - '0');
					if(nr)
						flags = flags & 0xC03F | ((((u32)flags << 18 >> 24) | (1 << nr)) << 6);
				}
				u32 tag = *(u32*) ch1;
				if(tag == *(u32*)"_ml0")
					flags |= LAYER_ML0;
				if(tag == *(u32*)"_ml1")
					flags |= LAYER_ML1;
				if(tag == *(u32*)"_mpu")
					flags |= LAYER_MPU;
				if(tag == *(u32*)"_ctf")
					flags |= LAYER_CTF;
			}

			if(!p || flags & layer_mask)
				keep = 1;
			if(!keep) {
				printf("filtering node '%s'\n", node->name);
				node->enabled = 0;
			} else if(node->name[0] == '_') {
				printf("not filtering node '%s'\n", node->name);
			}
		}
	}
}

void load_model(CModel** model, const char* filename, int flags)
{
	u8* data = NULL;
	int size = 0;

	printf("loading model %s\n", filename);
	if(flags & USE_ARCHIVE) {
		size = LoadFileFromArchive((void**)&data, filename);
	} else {
		size = LoadFile((void**)&data, filename);
	}

	*model = CModel_load(data, size, data, size, 0xFFFFFFFF);

	free_to_heap(data);
}

void load_room_model(CModel** model, const char* filename, const char* txtrfilename, int flags, int layer_mask)
{
	u8* data = NULL;
	int size = 0;
	u8* txtr = NULL;
	int txtrsz = 0;

	printf("loading model %s\n", filename);
	if(flags & USE_ARCHIVE) {
		size = LoadFileFromArchive((void**)&data, filename);
	} else {
		size = LoadFile((void**)&data, filename);
	}

	if(flags & USE_EXTERNAL_TXTR) {
		txtrsz = LoadFile((void**)&txtr, txtrfilename);
	} else {
		txtr = data;
		txtrsz = size;
	}

	*model = CModel_load(data, size, txtr, txtrsz, layer_mask);

	if(flags & USE_EXTERNAL_TXTR) {
		free_to_heap(txtr);
	}

	free_to_heap(data);
}

CModel* CModel_load(u8* scenedata, unsigned int scenesize, u8* texturedata, unsigned int texturesize, int layer_mask)
{
	unsigned int i, j;

	CModel* scene = (CModel*) malloc(sizeof(CModel));
	if(!scene)
		fatal("not enough memory");

	scene->animation = NULL;
	scene->node_animation = NULL;
	scene->texcoord_animations = NULL;
	scene->material_animations = NULL;

	HEADER* rawheader = (HEADER*) scenedata;

	scene->scale		= FX_FX32_TO_F32(get32bit_LE((u8*)&rawheader->scale));
	scene->scale		*= 1 << get32bit_LE((u8*)&rawheader->modelview_mtx_shamt);

	Material* materials	= (Material*)	((uintptr_t)scenedata + (uintptr_t)get32bit_LE((u8*)&rawheader->materials));
	Dlist* dlists		= (Dlist*)	((uintptr_t)scenedata + (uintptr_t)get32bit_LE((u8*)&rawheader->dlists));
	Node* nodes		= (Node*)	((uintptr_t)scenedata + (uintptr_t)get32bit_LE((u8*)&rawheader->nodes));
	Mesh* meshes		= (Mesh*)	((uintptr_t)scenedata + (uintptr_t)get32bit_LE((u8*)&rawheader->meshes));
	Texture* textures	= (Texture*)	((uintptr_t)scenedata + (uintptr_t)get32bit_LE((u8*)&rawheader->textures));
	Palette* palettes	= (Palette*)	((uintptr_t)scenedata + (uintptr_t)get32bit_LE((u8*)&rawheader->palettes));
	VecFx32* node_pos	= (VecFx32*)	((uintptr_t)scenedata + (uintptr_t)get32bit_LE((u8*)&rawheader->node_pos));
	VecFx32* node_init_pos	= (VecFx32*)	((uintptr_t)scenedata + (uintptr_t)get32bit_LE((u8*)&rawheader->node_initial_pos));

	scene->num_textures	= get16bit_LE((u8*)&rawheader->num_textures);
	scene->num_palettes	= get16bit_LE((u8*)&rawheader->num_palettes);
	scene->num_materials	= get16bit_LE((u8*)&rawheader->num_materials);
	scene->num_nodes	= get16bit_LE((u8*)&rawheader->num_nodes);
	scene->num_meshes	= get16bit_LE((u8*)&rawheader->num_meshes);

	scene->materials = NULL;
	if(rawheader->materials) {
		scene->materials = (CMaterial*) malloc(scene->num_materials * sizeof(CMaterial));
	}

	scene->textures = NULL;
	if(rawheader->textures) {
		scene->textures = (CTexture*) malloc(scene->num_textures * sizeof(CTexture));
		if(!scene->textures)
			fatal("not enough memory");
	}

	scene->nodes = NULL;
	if(rawheader->nodes) {
		scene->nodes = (CNode*) malloc(scene->num_nodes * sizeof(CNode));
	}

	scene->node_pos = NULL;
	if(rawheader->node_pos) {
		scene->node_pos = (Vec3*) malloc(scene->num_nodes * sizeof(CNode));
		for(i = 0; i < scene->num_nodes; i++) {
			scene->node_pos[i].x = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&node_pos[i].x));
			scene->node_pos[i].y = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&node_pos[i].y));
			scene->node_pos[i].z = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&node_pos[i].z));
		}
	}

	scene->node_initial_pos = NULL;
	if(rawheader->node_initial_pos) {
		scene->node_initial_pos = (Vec3*) malloc(scene->num_nodes * sizeof(CNode));
		for(i = 0; i < scene->num_nodes; i++) {
			scene->node_initial_pos[i].x = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&node_init_pos[i].x));
			scene->node_initial_pos[i].y = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&node_init_pos[i].y));
			scene->node_initial_pos[i].z = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&node_init_pos[i].z));
		}
	}

	if(rawheader->nodes) {
		for(i = 0; i < scene->num_nodes; i++) {
			CNode* node = &scene->nodes[i];
			Node* raw = &nodes[i];

			strncpy(node->name, raw->name, 64);
			node->parent = (s16) get16bit_LE((u8*)&raw->parent);
			node->child = (s16) get16bit_LE((u8*)&raw->child);
			node->next = (s16) get16bit_LE((u8*)&raw->next);
			node->mesh_count = get16bit_LE((u8*)&raw->mesh_count);
			node->mesh_id = (s16) get16bit_LE((u8*)&raw->mesh_id);
			node->enabled = get32bit_LE((u8*)&raw->enabled);
			node->type = raw->type;
			node->scale.x = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&raw->scale.x));
			node->scale.y = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&raw->scale.y));
			node->scale.z = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&raw->scale.z));
			node->angle.x = get16bit_LE((u8*)&raw->angle_x) / 65536.0 * 2.0 * M_PI;
			node->angle.y = get16bit_LE((u8*)&raw->angle_y) / 65536.0 * 2.0 * M_PI;
			node->angle.z = get16bit_LE((u8*)&raw->angle_z) / 65536.0 * 2.0 * M_PI;
			node->pos.x = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&raw->pos.x));
			node->pos.y = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&raw->pos.y));
			node->pos.z = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&raw->pos.z));
			node->offset = FX_FX32_TO_F32((fx32) get32bit_LE((u8*)&raw->offset));
			node->offset_raw = (fx32) get32bit_LE((u8*)&raw->offset);
		}

		CModel_filter_nodes(scene, layer_mask);
	}

	scene->num_node_weight = rawheader->num_node_weight;
	if (scene->num_node_weight > 0 && rawheader->node_weights) {
		int* ids = (int*)((uintptr_t)scenedata + (uintptr_t)get32bit_LE((u8*)&rawheader->node_weights));
		scene->node_weight_ids = (int*)malloc(scene->num_node_weight * sizeof(int));
		for (int i = 0; i < scene->num_node_weight; i++) {
			scene->node_weight_ids[i] = ids[i];
		}
	}

	printf("scale: %f\n", scene->scale);
	printf("%d materials, %d palettes, %d textures\n", scene->num_materials, scene->num_palettes, scene->num_textures);

	if(rawheader->materials && !scene->materials)
		fatal("not enough memory");

	if(rawheader->materials) {
		for(i = 0; i < scene->num_materials; i++) {
			Material* m = &materials[i];
			m->palid = get16bit_LE((u8*)&m->palid);
			m->texid = get16bit_LE((u8*)&m->texid);

			CMaterial* mat = &scene->materials[i];
			strcpy(mat->name, m->name);
			mat->render_mode = m->render_mode;
			//if(mat->render_mode > 2)
			//	mat->render_mode = TRANSLUCENT;
			mat->texid = m->texid;
			mat->light = m->light;
			mat->culling = m->culling;
			mat->alpha = m->alpha;
			mat->anim_flags = m->anim_flags;
			mat->x_repeat = m->x_repeat;
			mat->y_repeat = m->y_repeat;
			mat->polygon_mode = get32bit_LE((u8*)&m->polygon_mode);
			mat->scale_s = FX_FX32_TO_F32(get32bit_LE((u8*)&m->scale_s));
			mat->scale_t = FX_FX32_TO_F32(get32bit_LE((u8*)&m->scale_t));
			mat->translate_s = FX_FX32_TO_F32(get32bit_LE((u8*)&m->translate_s));
			mat->translate_t = FX_FX32_TO_F32(get32bit_LE((u8*)&m->translate_t));
			mat->rot_z = FX_FX32_TO_F32(FX_IDX_TO_RAD(get16bit_LE((u8*)&m->rot_z)));
			mat->diffuse = m->diffuse;
			mat->ambient = m->ambient;
			mat->specular = m->specular;
			// mat->diffuse.r = (u8) (m->diffuse.r / (float)0x1F * 255.0);
			// mat->diffuse.g = (u8) (m->diffuse.g / (float)0x1F * 255.0);
			// mat->diffuse.b = (u8) (m->diffuse.b / (float)0x1F * 255.0);
			// mat->ambient.r = (u8) (m->ambient.r / (float)0x1F * 255.0);
			// mat->ambient.g = (u8) (m->ambient.g / (float)0x1F * 255.0);
			// mat->ambient.b = (u8) (m->ambient.b / (float)0x1F * 255.0);
			// mat->specular.r = (u8) (m->specular.r / (float)0x1F * 255.0);
			// mat->specular.g = (u8) (m->specular.g / (float)0x1F * 255.0);
			// mat->specular.b = (u8) (m->specular.b / (float)0x1F * 255.0);

			mat->texgen_mode = get32bit_LE((u8*)&m->texcoord_transform_mode);
			mat->matrix_id = get32bit_LE((u8*)&m->matrix_id);
			mat->palid = m->palid;
			mat->texid = m->texid;
			printf("material %d: render mode is %d\n", i, mat->render_mode);
		}
	}

	scene->palettes = NULL;
	if(rawheader->palettes) {
		scene->palettes = (CPalette*) malloc(scene->num_palettes * sizeof(CPalette));
		if(!scene->palettes)
			fatal("not enough memory");

		for(i = 0; i < scene->num_palettes; i++) {
			CPalette* pal = &scene->palettes[i];
			u32 entries_ofs = get32bit_LE((u8*)&palettes[i].entries_ofs);
			pal->size = get32bit_LE((u8*)&palettes[i].count);
			if(entries_ofs >= texturesize) {
				printf("invalid paxel offset for palette %d\n", i, entries_ofs);
				continue;
			}

			pal->data = (u16*) malloc(pal->size);
			u16* src = (u16*) ((uintptr_t)texturedata + (uintptr_t)entries_ofs);
			for(j = 0; j < pal->size / 2; j++) {
				pal->data[j] = get16bit_LE((u8*)&src[j]);
			}
		}
	}

	if(rawheader->textures) {
		for(i = 0; i < scene->num_textures; i++) {
			CTexture* tex = &scene->textures[i];
			Texture* t = &textures[i];
			t->width = get16bit_LE((u8*)&t->width);
			t->height = get16bit_LE((u8*)&t->height);
			t->image_ofs = get32bit_LE((u8*)&t->image_ofs);
			t->imagesize = get32bit_LE((u8*)&t->imagesize);
			tex->format = get16bit_LE((u8*)&t->format);
			tex->width = t->width;
			tex->height = t->height;
			tex->opaque = t->opaque;

			tex->data = NULL;

			if(t->image_ofs >= texturesize) {
				printf("invalid texel offset for texture %d\n", i);
				continue;
			}

			u8* texels = (u8*) ((uintptr_t)texturedata + (uintptr_t)t->image_ofs);
			tex->data = (u8*) malloc(t->imagesize);
			if(!tex->data)
				fatal("not enough memory");
			memcpy(tex->data, texels, t->imagesize);
		}
	}

	if(rawheader->meshes) {
		scene->num_dlists = 0;
		for(i = 0; i < scene->num_meshes; i++) {
			Mesh* mesh = &meshes[i];
			mesh->matid = get16bit_LE((u8*)&mesh->matid);
			mesh->dlistid = get16bit_LE((u8*)&mesh->dlistid);
			if(mesh->dlistid >= scene->num_dlists)
				scene->num_dlists = mesh->dlistid + 1;
			Dlist* dlist = &dlists[mesh->dlistid];
			dlist->start_ofs = get32bit_LE((u8*)&dlist->start_ofs);
			dlist->size = get32bit_LE((u8*)&dlist->size);
			u32* data = (u32*) (scenedata + dlist->start_ofs);
			u32* end = data + dlist->size / 4;
			while(data < end) {
				*data = get32bit_LE((u8*)data);
				data++;
			}
		}

		scene->dlists = (int*) malloc(scene->num_dlists * sizeof(int));
		for(i = 0; i < scene->num_dlists; i++)
			scene->dlists[i] = -1;
	} else {
		scene->dlists = NULL;
	}

	if(scene->textures) {
		make_textures(scene);
	}

	if(rawheader->meshes) {
		build_meshes(scene, meshes, dlists, scene->num_meshes, scenedata);
	}

	if(rawheader->nodes) {
		scene->apply_transform = 1;
		CModel_compute_node_matrices(scene, 0);
	}

#if 0
	if(rawheader->num_texture_matrices) {
		u16 cnt = get16bit_LE((u8*)&rawheader->num_texture_matrices);
		printf("allocating %d texture matrices\n", cnt);
		scene->texture_matrices = (Mtx44*) alloc_from_heap(cnt * sizeof(Mtx44));
		for(i = 0; i < cnt; i++)
			MTX44Identity(&scene->texture_matrices[i]);
	} else
#endif
	scene->texture_matrices = NULL;
	scene->light_override = false;
	return scene;
}

void CModel_set_textures(CModel* model)
{
	if(!model->textures)
		return;

	make_textures(model);
}

void CModel_set_texture_filter(CModel* model, int type)
{
	u32 m;
	for(m = 0; m < model->num_materials; m++) {
		if(model->materials[m].texid != 0xFFFF) {
			glBindTexture(GL_TEXTURE_2D, model->materials[m].tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);
		}
	}
}


void scale_rotate_translate(Mtx44* mtx, float sx, float sy, float sz, float ax, float ay, float az, float x, float y, float z)
{
	float sin_ax = sinf(ax);
	float sin_ay = sinf(ay);
	float sin_az = sinf(az);
	float cos_ax = cosf(ax);
	float cos_ay = cosf(ay);
	float cos_az = cosf(az);

	float v18 = cos_ax * cos_az;
	float v19 = cos_ax * sin_az;
	float v20 = cos_ax * cos_ay;

	float v22 = sin_ax * sin_ay;

	float v17 = v19 * sin_ay;

	mtx->m[0][0] = sx * cos_ay * cos_az;
	mtx->m[0][1] = sx * cos_ay * sin_az;
	mtx->m[0][2] = sx * -sin_ay;

	mtx->m[1][0] = sy * ((v22 * cos_az) - v19);
	mtx->m[1][1] = sy * ((v22 * sin_az) + v18);
	mtx->m[1][2] = sy * sin_ax * cos_ay;

	mtx->m[2][0] = sz * (v18 * sin_ay + sin_ax * sin_az);
	mtx->m[2][1] = sz * ((v17 + (v19 * sin_ay)) - (sin_ax * cos_az));
	mtx->m[2][2] = sz * v20;

	mtx->m[3][0] = x;
	mtx->m[3][1] = y;
	mtx->m[3][2] = z;

	mtx->m[0][3] = 0;
	mtx->m[1][3] = 0;
	mtx->m[2][3] = 0;
	mtx->m[3][3] = 1;
}

void CModel_compute_node_matrices(CModel* model, int start_idx)
{
	int idx;
	Mtx44 transform;
	CNode* node;

	if(!model->nodes || start_idx == -1)
		return;

	for(idx = start_idx; idx != -1; idx = node->next) {
		node = &model->nodes[idx];

#if 1
		/* NOTE: this fixes translations together with model scale, but it's *broken* in-game */
		scale_rotate_translate(&transform, node->scale.x, node->scale.y, node->scale.z,
				node->angle.x, node->angle.y, node->angle.z,
				node->pos.x / model->scale, node->pos.y / model->scale, node->pos.z / model->scale);
#else
		scale_rotate_translate(&transform, node->scale.x, node->scale.y, node->scale.z,
				node->angle.x, node->angle.y, node->angle.z,
				node->pos.x, node->pos.y, node->pos.z);
#endif

		if(node->parent == -1)
			MTX44Copy(&transform, &node->node_transform);
		else
			MTX44Concat(&model->nodes[node->parent].node_transform, &transform, &node->node_transform);

		if(node->child != -1)
			CModel_compute_node_matrices(model, node->child);

		if(model->node_pos) {
			MTX44Identity(&transform);
			transform.m[3][0] = -model->node_initial_pos[idx].x;
			transform.m[3][1] = -model->node_initial_pos[idx].y;
			transform.m[3][2] = -model->node_initial_pos[idx].z;
			MTX44Concat(&node->node_transform, &transform, &node->node_transform);
		}
	}
}

CModel* CModel_load_file(const char* model, const char* textures, int layer_mask)
{
	FILE* file = fopen(model, "rb");
	if(!file) {
		printf("%s not found\n", model);
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	u32 scenesize = ftell(file);
	fseek(file, 0, SEEK_SET);
	u8* content = (u8*) malloc(scenesize);
	fread(content, 1, scenesize, file);
	fclose(file);

	u32 texturesize = scenesize;
	u8* texturedata = content;

	if(textures != NULL) {
		file = fopen(textures, "rb");
		if(!file) {
			printf("%s not found\n", textures);
			exit(1);
		}
		fseek(file, 0, SEEK_END);
		texturesize = ftell(file);
		fseek(file, 0, SEEK_SET);
		texturedata = (u8*) malloc(texturesize);
		fread(texturedata, 1, texturesize, file);
		fclose(file);
	}

	CModel* scene = CModel_load(content, scenesize, texturedata, texturesize, layer_mask);
	if(texturedata != content)
		free(texturedata);
	free(content);
	return scene;
}

void CModel_free(CModel* scene)
{
	unsigned int i;
	for(i = 0; i < scene->num_materials; i++) {
		glDeleteTextures(1, &scene->materials[i].tex);
	}
	for(i = 0; i < scene->num_dlists; i++) {
		glDeleteLists(1, scene->dlists[i]);
	}
	for(i = 0; i < scene->num_textures; i++) {
		free(scene->textures[i].data);
	}
	for(i = 0; i < scene->num_palettes; i++) {
		free(scene->palettes[i].data);
	}
	free(scene->textures);
	free(scene->palettes);
	free(scene->materials);
	free(scene->meshes);
	free(scene->dlists);
	free(scene->nodes);
	free(scene->node_pos);
	free(scene->node_initial_pos);
	free(scene);
}

static void use_room_lights()
{
	glUniform3fv(light1vec, 1, l1v);
	glUniform3fv(light1col, 1, l1c);
	glUniform3fv(light2vec, 1, l2v);
	glUniform3fv(light2col, 1, l2c);
}

extern bool lighting;

static CNode* current_node;

extern bool texturing;

extern float pos_x;
extern float pos_y;
extern float pos_z;
void CModel_render_mesh(CModel* scene, int mesh_id, Mtx44* transform)
{
	if(mesh_id >= scene->num_meshes) {
		printf("trying to render mesh %d, but scene only has %d meshes\n", mesh_id, scene->num_meshes);
		return;
	}

	CMesh* mesh = &scene->meshes[mesh_id];
	CMaterial* orig_mtl = &scene->materials[mesh->matid];
	CTexture* texture = &scene->textures[orig_mtl->texid];

	CMaterial material = *orig_mtl;

	if(scene->material_animations && material.material_anim_id != -1) {
		process_material_animation(scene->material_animations, material.material_anim_id, &material);
	}

	float diff[3] = { material.diffuse.r / 31.0f, material.diffuse.g / 31.0f, material.diffuse.b / 31.0f };
	glColor3fv(diff);

	if(material.texid != 0xFFFF) {
		Mtx44 texcoord;

		glBindTexture(GL_TEXTURE_2D, material.tex);

		if(scene->texcoord_animations && material.texcoord_anim_id != -1) {
			process_texcoord_animation(scene->texcoord_animations, material.texcoord_anim_id, texture->width, texture->height, &texcoord);
			MTX44ScaleApply(&texcoord, &texcoord, 1.0f / texture->width, 1.0f / texture->height, 1.0f);
		} else if(material.texgen_mode != GX_TEXGEN_NONE) {
			if(scene->texture_matrices) {
				MTX44Copy(&scene->texture_matrices[material.matrix_id], &texcoord);
				MTX44ScaleApply(&texcoord, &texcoord, 1.0f / texture->width, 1.0f / texture->height, 1.0f);
			} else {
				if(material.rot_z != 0)
					MTX44RotRad(&texcoord, 'z', material.rot_z);
				else
					MTX44Identity(&texcoord);
				MTX44TransApply(&texcoord, &texcoord, material.translate_s * texture->width, material.translate_t * texture->height, 0.0f);
				MTX44ScaleApply(&texcoord, &texcoord, material.scale_s, material.scale_t, 1.0f);
				MTX44ScaleApply(&texcoord, &texcoord, 1.0f / texture->width, 1.0f / texture->height, 1.0f);
			}
		} else {
			MTX44Scale(&texcoord, 1.0f / texture->width, 1.0f / texture->height, 1.0f);
		}

		glUniform1i(use_texture, texturing);
		glUniformMatrix4fv(texcoord_matrix, 1, 0, texcoord.a);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(use_texture, 0);
	}

	if(lighting && material.light) {
		float amb[3] = { (float)material.ambient.r / 31.0f, (float)material.ambient.g / 31.0f, (float)material.ambient.b / 31.0f };
		float spec[3] = { material.specular.r / 31.0f, material.specular.g / 31.0f, material.specular.b / 31.0f };
		glEnable(GL_LIGHTING);
		glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
		glUniform3fv(ambient, 1, amb);
		glUniform3fv(diffuse, 1, diff);
		glUniform3fv(specular, 1, spec);
		glUniform1i(use_light, 1);
		if (scene->light_override) {
			Mtx44 light_transform;
			Vec3 pos;
			Vec3 vec1;
			Vec3 vec2;
			Vec3 octo_vec1;
			Vec3 octo_vec2;
			Vec3 light_vec;
			pos.x = transform->m[3][0];
			pos.y = transform->m[3][1];
			pos.z = transform->m[3][2];
			vec1.x = 0;
			vec1.y = 1;
			vec1.z = 0;
			vec2.x = pos_x - pos.x;
			vec2.y = 0;
			vec2.z = pos_z - pos.z;
			VEC_Normalize3(&vec2, &vec2);
			octo_vec1.x = 0;
			octo_vec1.y = 0.3005371f;
			octo_vec1.z = -0.5f;
			octo_vec2.x = 0;
			octo_vec2.y = 0;
			octo_vec2.z = -0.5f;
			get_transform_mtx3(&light_transform, &vec2, &vec1);
			MTX44MultVec(&light_transform, &octo_vec1, &light_vec);
			VEC_Normalize3(&light_vec, &light_vec);
			l1v_override[0] = light_vec.x;
			l1v_override[1] = light_vec.y;
			l1v_override[2] = light_vec.z;
			glUniform3fv(light1vec, 1, l1v_override);
			MTX44MultVec(&light_transform, &octo_vec2, &light_vec);
			VEC_Normalize3(&light_vec, &light_vec);
			l2v_override[0] = light_vec.x;
			l2v_override[1] = light_vec.y;
			l2v_override[2] = light_vec.z;
			glUniform3fv(light2vec, 1, l2v_override);
			l1c_override[0] = 1;
			l1c_override[1] = 1;
			l1c_override[2] = 1;
			l2c_override[0] = 1;
			l2c_override[1] = 1;
			l2c_override[2] = 1;
			glUniform3fv(light1col, 1, l1c_override);
			glUniform3fv(light2col, 1, l2c_override);
		} else {
			use_room_lights();
		}
	} else {
		glUniform1i(use_light, 0);
	}

	switch(material.culling) {
		case DOUBLE_SIDED:
			glDisable(GL_CULL_FACE);
			break;
		case BACK_SIDE:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case FRONT_SIDE:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
	}

	glCallList(scene->dlists[mesh->dlistid]);

	if(lighting) {
		glDisable(GL_LIGHTING);
	}
}

static void CModel_update_uniforms()
{
	use_room_lights();
	glUniform1i(fog_enable, fogen && !fogdis);
	glUniform4fv(fog_color, 1, fogcol);
	glUniform1f(fog_min, fogmin);
	glUniform1f(fog_max, fogmax);
	glUniform1f(alpha_scale, 1.0f);
	glUniformMatrix4fv(proj_matrix, 1, 0, projection.a);
	glUniformMatrix4fv(view_matrix, 1, 0, view.a);
	glUniform3fv(toon_table, TOON_SIZE, toon_values);
}

typedef struct RenderEntity RenderEntity;
struct RenderEntity {
	RenderEntity*	next;
	Mtx44		transform;
	Mtx44*		mtx_stack;
	CModel*		model;
	CNode*		node;
	int		mesh;
	float		alpha;
	float		mat_alpha;
	int		mode;
	int		poly_mode;
	unsigned int	polygon_id;
};

typedef struct MatrixStack MatrixStack;
struct MatrixStack {
	MatrixStack* next;
	Mtx44* matrices;
};

static unsigned int next_polygon_id;
static unsigned int render_count;
static RenderEntity* render_list;
static RenderEntity* last_render_node;
static MatrixStack* matrix_stacks;

static void RenderEntity_get_position(RenderEntity* ent, Vec3* pos)
{
	float cx = (ent->model->max_x + ent->model->min_x) / 2.0;
	float cy = (ent->model->max_y + ent->model->min_y) / 2.0;
	float cz = (ent->model->max_z + ent->model->min_z) / 2.0;
	Vec3 pt = { cx, cy, cz };
	MTX44MultVec(&ent->transform, &pt, pos);
}

static float RenderEntity_get_distance(RenderEntity* ent)
{
	Vec3 pos;
	RenderEntity_get_position(ent, &pos);
	float dx = pos.x - pos_x;
	float dy = pos.y - pos_y;
	float dz = pos.z - pos_z;
	return -sqrtf(dx * dx + dy * dy + dz * dz);
}

static int RenderEntity_sort(const void* a, const void* b)
{
	RenderEntity* x = *(RenderEntity**) a;
	RenderEntity* y = *(RenderEntity**) b;

	float dst1 = RenderEntity_get_distance(x);
	float dst2 = RenderEntity_get_distance(y);

	if(dst1 == dst2)
		return 0;
	else if(dst1 < dst2)
		return 1;
	else
		return -1;
}

void CModel_begin_scene(void)
{
	render_list = NULL;
	last_render_node = NULL;
	matrix_stacks = NULL;
	render_count = 0;
	next_polygon_id = 1;
}

static void RenderEntity_render(RenderEntity* ent)
{
	glUniform1i(mat_mode, ent->poly_mode);
	glUniform1f(alpha_scale, ent->alpha);
	glUniform1f(mat_alpha, ent->mat_alpha / 31.0f);
	current_node = ent->node;
	if (ent->model->num_node_weight == 0) {
		glUniformMatrix4fv(matrix_stack, 1, 0, &ent->transform);
	} else {
		glUniformMatrix4fv(matrix_stack, ent->model->num_node_weight, 0, ent->mtx_stack);
	}
	CModel_render_mesh(ent->model, ent->mesh, &ent->transform);
}

void CModel_end_scene(void)
{
	if(!render_count)
		return;

	unsigned int polygon_id = 4;
	RenderEntity* ent = render_list;
	RenderEntity** sorted = (RenderEntity**)alloc_from_heap(render_count * sizeof(RenderEntity*));
	unsigned int i = 0;

	// collect all nodes
	while(ent) {
		sorted[i++] = ent;
		ent = ent->next;
	}

	// depth sort
	// qsort(sorted, render_count, sizeof(RenderEntity*), RenderEntity_sort);

	glUseProgram(shader);

	CModel_update_uniforms();

	//////////////////////////////////////////////////////////////////
	// pass 1: opaque
	//////////////////////////////////////////////////////////////////
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_EQUAL, 1.0f);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
	glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	for(i = 0; i < render_count; i++) {
		RenderEntity* ent = sorted[i];
		if(ent->mode != DECAL)
			RenderEntity_render(ent);
	}
	glDisable(GL_ALPHA_TEST);

	//////////////////////////////////////////////////////////////////
	// pass 2: decal
	//////////////////////////////////////////////////////////////////
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1, -1);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(i = 0; i < render_count; i++) {
		RenderEntity* ent = sorted[i];
		if(ent->mode == DECAL)
			RenderEntity_render(ent);
	}
	glPolygonOffset(0, 0);
	glDisable(GL_POLYGON_OFFSET_FILL);

	//////////////////////////////////////////////////////////////////
	// pass 3: mark transparent faces in stencil
	//////////////////////////////////////////////////////////////////
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_LESS, 1.0f);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	for(i = 0; i < render_count; i++) {
		RenderEntity* ent = sorted[i];
		if(ent->mode >= TRANSLUCENT) {
			glStencilFunc(GL_GREATER, ent->polygon_id, 0xFF);
			RenderEntity_render(ent);
		}
	}

	//////////////////////////////////////////////////////////////////
	// pass 4: rebuild depth buffer
	//////////////////////////////////////////////////////////////////
	glClear(GL_DEPTH_BUFFER_BIT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glAlphaFunc(GL_EQUAL, 1.0f);
	for(i = 0; i < render_count; i++) {
		RenderEntity* ent = sorted[i];
		if(ent->mode != DECAL)
			RenderEntity_render(ent);
	}

	//////////////////////////////////////////////////////////////////
	// pass 5: translucent (behind)
	//////////////////////////////////////////////////////////////////
	glAlphaFunc(GL_LESS, 1.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	for(i = 0; i < render_count; i++) {
		RenderEntity* ent = sorted[i];
		if(ent->mode >= TRANSLUCENT) {
			glStencilFunc(GL_NOTEQUAL, ent->polygon_id, 0xFF);
			RenderEntity_render(ent);
		}
	}

	//////////////////////////////////////////////////////////////////
	// pass 6: translucent (before)
	//////////////////////////////////////////////////////////////////
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	for(i = 0; i < render_count; i++) {
		RenderEntity* ent = sorted[i];
		if(ent->mode >= TRANSLUCENT) {
			glStencilFunc(GL_EQUAL, ent->polygon_id, 0xFF);
			RenderEntity_render(ent);
		}
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_STENCIL_TEST);

	glUseProgram(0);

	// release
	for(i = 0; i < render_count; i++) {
		free_to_heap(sorted[i]);
	}

	MatrixStack* stack = matrix_stacks;
	while (stack) {
		struct MatrixStack* next = stack->next;
		free_to_heap(stack);
		stack = next;
	}

	free_to_heap(sorted);
	render_list = NULL;
	last_render_node = NULL;
	matrix_stacks = NULL;
}

void CModel_add_model(CModel* scene, Mtx44* mtx, MatrixStack* mtx_stack, CNode* node, int mesh, float alpha, float mat_alpha, int mode, int poly_mode, int polygon_id)
{
	RenderEntity* ent = (RenderEntity*)alloc_from_heap(sizeof(RenderEntity));
	if (mtx_stack) {
		ent->mtx_stack = mtx_stack->matrices;
	} else {
		MTX44Copy(mtx, &ent->transform);
	}
	ent->model = scene;
	ent->node = node;
	ent->mesh = mesh;
	ent->alpha = alpha;
	ent->mat_alpha = mat_alpha;
	ent->mode = mode;
	ent->poly_mode = poly_mode;
	ent->polygon_id = polygon_id;
	ent->next = NULL;
	if(!render_list) {
		render_list = ent;
		last_render_node = ent;
	} else {
		last_render_node->next = ent;
		last_render_node = ent;
	}
	render_count++;
	if(polygon_id > 255)
		printf("ERROR! %d\n", polygon_id);
}

void CModel_render_all(CModel* scene, Mtx44* mtx, float alpha)
{
	Mtx44 mat;
	unsigned int i, j;

	MTX44Scale(&mat, scene->scale, scene->scale, scene->scale);
	MTX44Concat(mtx, &mat, &mat);

	if(scene->node_animation) {
		process_node_animation(scene->node_animation, &mat, scene->scale);
	}

	unsigned int polygon_id = next_polygon_id++;
	if(next_polygon_id > 255)
		next_polygon_id = 0;

	MatrixStack* stack = NULL;
	if (scene->num_node_weight > 0) {
		stack = (MatrixStack*)malloc(sizeof(MatrixStack));
		stack->matrices = (Mtx44*)malloc(scene->num_node_weight * sizeof(Mtx44));
		for (int i = 0; i < scene->num_node_weight; i++) {
			MTX44Identity(&stack->matrices[i]);
		}
		stack->next = matrix_stacks;
		matrix_stacks = stack;
	}

	for(i = 0; i < scene->num_nodes; i++) {
		Mtx44 transform;
		Mtx44 billboard;
		CNode* node = &scene->nodes[i];
		current_node = node;

		if(scene->node_animation) {
			MTX44Copy(&node->node_transform, &transform);
		} else if(scene->apply_transform) {
			MTX44Concat(&mat, &node->node_transform, &transform);
		} else {
			MTX44Copy(&mat, &transform);
		}

		if (node->type) {
			MTX44ClearRot(&transform, &billboard);
			if (node->type == 1) {
				MTX44Concat(&billboard, &view_inv_xyrot, &billboard);
			} else if (node->type == 2) {
				MTX44Concat(&billboard, &view_inv_yrot, &billboard);
			}
			MTX44Copy(&billboard, &transform);
		}

		for (j = 0; j < scene->num_node_weight; j++) {
			if (scene->node_weight_ids[j] == i) {
				MTX44Copy(&transform, &stack->matrices[j]);
				break;
			}
		}

		if (node->mesh_count) {
			int mesh_id = node->mesh_id / 2;
			for(j = 0; j < node->mesh_count; j++) {
				int id = mesh_id + j;
				CMesh* mesh = &scene->meshes[id];
				CMaterial* material = &scene->materials[mesh->matid];
				CModel_add_model(scene, &transform, stack, node, id, alpha, material->alpha, material->render_mode, material->polygon_mode, polygon_id);
			}
		}
	}
}

void CModel_render_node(CModel* scene, Mtx44* mtx, int node_idx, float alpha)
{
	Mtx44 mat;
	unsigned int i, j;

	MTX44Scale(&mat, scene->scale, scene->scale, scene->scale);
	MTX44Concat(mtx, &mat, &mat);

	for(i = node_idx; i != -1; i = scene->nodes[i].next) {
		Mtx44 transform;
		CNode* node = &scene->nodes[i];
		if(node->mesh_count > 0 && node->enabled == 1) {
			int mesh_id = node->mesh_id / 2;

			if(scene->apply_transform) {
				MTX44Concat(&mat, &node->node_transform, &transform);
			} else {
				MTX44Copy(&mat, &transform);
			}

			for(j = 0; j < node->mesh_count; j++) {
				int id = mesh_id + j;
				CMesh* mesh = &scene->meshes[id];
				CMaterial* material = &scene->materials[mesh->matid];
				unsigned int polygon_id = 0;
				if(material->render_mode >= TRANSLUCENT)
					polygon_id = next_polygon_id++;
				CModel_add_model(scene, &transform, NULL, node, id, alpha, material->alpha, material->render_mode, material->polygon_mode, polygon_id);
				if(next_polygon_id > 255)
					next_polygon_id = 0;
			}
		}
	}
}

const float toon_values[TOON_SIZE * 3] = {
	GetTableColor(0x2000),
	GetTableColor(0x2000),
	GetTableColor(0x2020),
	GetTableColor(0x2021),
	GetTableColor(0x2021),
	GetTableColor(0x2041),
	GetTableColor(0x2441),
	GetTableColor(0x2461),
	GetTableColor(0x2461),
	GetTableColor(0x2462),
	GetTableColor(0x2482),
	GetTableColor(0x2482),
	GetTableColor(0x28C3),
	GetTableColor(0x2CE4),
	GetTableColor(0x3105),
	GetTableColor(0x3546),
	GetTableColor(0x3967),
	GetTableColor(0x3D88),
	GetTableColor(0x41C9),
	GetTableColor(0x45EA),
	GetTableColor(0x4A0B),
	GetTableColor(0x4E4B),
	GetTableColor(0x526C),
	GetTableColor(0x568D),
	GetTableColor(0x5ACE),
	GetTableColor(0x5EEF),
	GetTableColor(0x6310),
	GetTableColor(0x6751),
	GetTableColor(0x6B72),
	GetTableColor(0x6F93),
	GetTableColor(0x73D4),
	GetTableColor(0x77F5)
};
