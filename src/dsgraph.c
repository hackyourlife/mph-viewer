#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <GL/glut.h>
#include <GL/glext.h>

#include <math.h>

#include "types.h"
#include "endianess.h"
#include "model.h"
#include "rooms.h"
#include "room.h"
#include "entity.h"
#include "game.h"

#define M_PI		3.14159265358979323846

long time = 0;

bool texturing = true;
bool colouring = true;
bool wireframe = false;
bool culling = true;
bool tex_filtering = true;
bool animate = true;
bool lighting = true;

float sin_deg(float deg) {
	return sin(deg * M_PI / 180);
}

float cos_deg(float deg) {
	return cos(deg * M_PI / 180);
}

bool mouse_r = false;
bool mouse_l = false;
int mouse_x = 0;
int mouse_y = 0;

float heading = 0.0f;
float heading_y = 0.0f;

float xrot = 0.0f;
float yrot = 0.0f;

float pos_x = 0.0f;
float pos_y = 0.0f;
float pos_z = 0.0f;

bool key_down_forward = false;
bool key_down_backward = false;
bool key_down_speed = false;

bool is_fullscreen = false;

bool fog_disable = false;
bool show_entities = true;
bool force_fields_active = true;

bool cleanup = false;

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

unsigned int frame_count = 0;
bool dump_frames = false;

void move_forward(float distance)
{
	pos_x -= distance * (float)sin_deg(heading) * cos_deg(heading_y) * 0.05f;
	pos_y -= distance * (float)sin_deg(heading_y) * 0.05f;
	pos_z -= distance * (float)cos_deg(heading) * cos_deg(heading_y) * 0.05f;
}

void move_backward(float distance)
{
	pos_x += distance * (float)sin_deg(heading) * cos_deg(heading_y) * 0.05f;
	pos_y += distance * (float)sin_deg(heading_y) * 0.05f;
	pos_z += distance * (float)cos_deg(heading) * cos_deg(heading_y) * 0.05f;
}

void mouse_func(int button, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	if(button == GLUT_RIGHT_BUTTON)
		mouse_r = state == GLUT_DOWN;
	if(button == GLUT_LEFT_BUTTON)
		mouse_l = state == GLUT_DOWN;
}

void motion_func(int x, int y)
{
	int dx = x - mouse_x;
	int dy = y - mouse_y;

	if( mouse_l && (dx || dy)) {
		if(dx || dy) {
			heading -= dx / 5.0f;
			if(heading > 360)	heading -= 360;
			if(heading < 0)		heading += 360;
			heading_y += dy / 5.0f;
			if(heading_y > 360)	heading_y -= 360;
			if(heading_y < 0)	heading_y += 360;
			xrot = heading_y;
			yrot = heading;
			glutPostRedisplay();
		}
	}

	mouse_x = x;
	mouse_y = y;
}

void special_func(int key, int x, int y)
{
	switch(key) {
		case GLUT_KEY_F2:
			if(!is_fullscreen) {
				glutFullScreen();
			} else {
				glutReshapeWindow(512, 512);
			}
			is_fullscreen = !is_fullscreen;
			break;
		case GLUT_KEY_UP:
			key_down_forward = true;
			break;

		case GLUT_KEY_DOWN:
			key_down_backward = true;
			break;
	}
}

void special_up_func(int key, int x, int y)
{
	switch(key) {
		case GLUT_KEY_UP:
			key_down_forward = false;
			break;

		case GLUT_KEY_DOWN:
			key_down_backward = false;
			break;
	}
}

bool depth_test = true;

void perform_cleanup(void)
{
	if(!cleanup) {
		cleanup = true;
		GAMEUnloadRoom();
	}
}

void kb_func(unsigned char key, int x, int y)
{
	switch(key) {
		case 0x1b:
			perform_cleanup();
			exit(0);
			break;
		case 't':	case 'T': {
			texturing = !texturing;
			if(texturing) {
				glEnable(GL_TEXTURE_2D);
			} else {
				glDisable(GL_TEXTURE_2D);
			}
			glutPostRedisplay();
		}
		break;

		case 'd':	case 'D': {
			depth_test = !depth_test;
			(depth_test ? glEnable : glDisable)(GL_DEPTH_TEST);
			glutPostRedisplay();
		}
		break;

		case 'c':	case 'C': {
			colouring = !colouring;
			glutPostRedisplay();
		}
		break;

		case 'w':	case 'W': {
			wireframe = !wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
			glutPostRedisplay();
		}
		break;

		case 'b':	case 'B': {
			culling = !culling;
			(culling ? glEnable : glDisable)(GL_CULL_FACE);
			glutPostRedisplay();
		}
		break;

		case 'f':	case 'F': {
			tex_filtering = !tex_filtering;
			CModel_set_texture_filter(room->model, tex_filtering ? GL_LINEAR : GL_NEAREST);
			EntSetTextureFilter(tex_filtering ? GL_LINEAR : GL_NEAREST);
			glutPostRedisplay();
		}
		break;

		case 'a':	case 'A': {
			animate = !animate;
			glutPostRedisplay();
		}
		break;

		case 'l':	case 'L': {
			lighting = !lighting;
			glutPostRedisplay();
		}
		break;

		case 'g':	case 'G': {
			fog_disable = !fog_disable;
			CModel_setFogDisable(fog_disable);
			glutPostRedisplay();
		}
		break;

		case 'e':	case 'E': {
			show_entities = !show_entities;
			glutPostRedisplay();
		}
		break;

		case 'p':	case 'P': {
			force_fields_active = !force_fields_active;
			for(CEntity* ent = CEntity_get_instances(FORCE_FIELD); ent; ent = ent->next) {
				CForceField_set_state(ent, force_fields_active ? 1 : 0);
			}
			glutPostRedisplay();
		}
		break;

		case 'r':	case 'R': {
			dump_frames = !dump_frames;
		}
		break;

		case ' ':
			key_down_speed = true;
			break;
	}
}

void kb_up_func(unsigned char key, int x, int y) {
	switch(key) {
		case ' ':
			key_down_speed = false;
			break;
	}
}


void process()
{
	long now = glutGet(GLUT_ELAPSED_TIME);
	float dt = (now - time) / 1000.0f;
	if(dump_frames)
		dt = 1.0 / 60.0;
	float distance = dt * 64.0f;
	if(key_down_speed)
		distance *= 10.0f;
	if(key_down_forward)
		move_forward(distance);
	if(key_down_backward)
		move_backward(distance);

	if(animate) {
		CRoom_process(room, dt);
		CEntity_process_all(dt);
	}

	time = now;
}

void display_func(void)
{
	process();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GLfloat vp[4];
	glGetFloatv(GL_VIEWPORT, vp);
	float aspect = (vp[2] - vp[0]) / (vp[3] - vp[1]);

	GAMERenderScene(aspect);

	glutSwapBuffers();

	if(dump_frames) {
		int width = vp[2] - vp[0];
		int height = vp[3] - vp[1];

		char filename[256];
		sprintf(filename, "framedump/frame-%04d.bmp", frame_count++);

		printf("dumping frame %d [%dx%d to %s]\n", frame_count, width, height, filename);

		FILE* f = fopen(filename, "wb");
		BMP header;
		BMP bmp;
		bmp.bfType = 0x4D42;
		bmp.bfSize = sizeof(BMP) + width * height * 3;
		bmp.bfReserved = 0;
		bmp.bfOffBits = sizeof(BMP);
		bmp.biSize = 40;
		bmp.biWidth = width;
		bmp.biHeight = height;
		bmp.biPlanes = 1;
		bmp.biBitCount = 24;
		bmp.biCompression = 0;
		bmp.biSizeImage = 0;
		bmp.biXPelsPerMeter = 0;
		bmp.biYPelsPerMeter = 0;
		bmp.biClrUsed = 0;
		bmp.biClrImportant = 0;
		fwrite(&bmp, sizeof(BMP), 1, f);

		void* buf = malloc(width * height * 3);
		glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, buf);
		fwrite(buf, width * height, 3, f);
		free(buf);

		fclose(f);
	}
}

int compute_mask(const char* mask)
{
	if(strlen(mask)) {
		int layer_mask = 0;
		unsigned int p;
		for(p = 0; p < strlen(mask); p += 4) {
			const char* ch1 = &mask[p];
			if(*ch1 != '_')
				break;
			if(*(u16*)ch1 == *(u16*)"_s") {
				int nr = mask[p + 3] - '0' + 10 * (mask[p + 2] - '0');
				if(nr)
					layer_mask = layer_mask & 0xC03F | ((((u32)layer_mask << 18 >> 24) | (1 << nr)) << 6);
			}
			u32 tag = *(u32*)ch1;
			if(tag == *(u32*)"_ml0")
				layer_mask |= LAYER_ML0;
			if(tag == *(u32*)"_ml1")
				layer_mask |= LAYER_ML1;
			if(tag == *(u32*)"_mpu")
				layer_mask |= LAYER_MPU;
			if(tag == *(u32*)"_ctf")
				layer_mask |= LAYER_CTF;
		}
		return layer_mask;
	} else {
		return 0xFFFFFFFF;
	}
}

int main(int argc, char **argv)
{
	char* modestring = NULL;
	int use_game_mode = 0;
	unsigned int layer_mask = 0;
	argc--;
	argv++;
	if(argc > 1 && !strcmp(argv[0], "-f")) {
		modestring = argv[1];
		argc -= 2;
		argv += 2;
	}
	if(argc != 1 && argc != 2) {
		printf("Metroid Prime Hunters model viewer\n");
		printf("Usage: dsgraph <id> [layer-mask]\n");
		exit(0);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL | GLUT_DOUBLE);
	if(modestring) {
		glutGameModeString(modestring);
		int possible = glutGameModeGet(GLUT_GAME_MODE_POSSIBLE);
		if(possible) {
			glutEnterGameMode();
			use_game_mode = 1;
		}
	}

	int room_id = atoi(argv[0]);
	if(argc > 1) {
		layer_mask = compute_mask(argv[1]);
		printf("overriding layer mask with 0x%x\n", layer_mask);
	}

	if(room_id < 0 || room_id >= NUM_ROOMS) {
		printf("invalid room id\n");
		return 0;
	}

	if(!use_game_mode) {
		glutInitWindowSize(512, 512);
		glutInitWindowPosition(100, 100);
		glutCreateWindow(rooms[room_id].name);
	}

	const char* gl_vendor = glGetString(GL_VENDOR);
	const char* gl_renderer = glGetString(GL_RENDERER);
	const char* gl_version = glGetString(GL_VERSION);
	const char* gl_glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("GL Vendor:    %s\n", gl_vendor);
	printf("GL Renderer:  %s\n", gl_renderer);
	printf("GL Version:   %s\n", gl_version);
	printf("GLSL Version: %s\n", gl_glsl_version);

	printf("using depth buffer with %d bit and stencil buffer with %d bit\n", glutGet(GLUT_WINDOW_DEPTH_SIZE), glutGet(GLUT_WINDOW_STENCIL_SIZE));

	glutDisplayFunc(display_func);
	glutIdleFunc(display_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);
	glutKeyboardFunc(kb_func);
	glutKeyboardUpFunc(kb_up_func);
	glutSpecialFunc(special_func);
	glutSpecialUpFunc(special_up_func);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearStencil(0);

	printf("loading room %d...\n", room_id);
	printf("Room name: %s\n", rooms[room_id].name);

	GAMEInit();
	CModel_init();
	GAMESetRoom(room_id, layer_mask);

	const char* name = get_current_room_name();
	if(name)
		glutSetWindowTitle(name);

	printf("layer mask: 0x%04x\n", room->layer_mask);
	printf("position: %f,%f,%f\n", FX_FX32_TO_F32(room->pos.x), FX_FX32_TO_F32(room->pos.y), FX_FX32_TO_F32(room->pos.z));

	printf(" - Hold left mouse button to look around\n");
	printf(" - Up/Down moves around\n");
	printf(" - T toggles texturing\n");
	printf(" - C toggles vertex colours\n");
	printf(" - W toggles wireframe\n");
	printf(" - B toggles backface culling\n");
	printf(" - F toggles texture filtering\n");
	printf(" - G toggles fog\n");
	printf(" - L toggles lighting\n");

	glutMainLoop();

	perform_cleanup();

	return 0;
}
