@echo off
gcc -g -o dsgraph -std=gnu99 -O3 -mno-ms-bitfields -Iinclude -Llib src/dsgraph.c src/model.c src/fs.c src/heap.c src/io.c src/texture_containers.c src/pickup_models.c src/rooms.c src/error.c src/os.c src/room.c src/entity.c src/jumppad.c src/teleporter.c src/object.c src/item.c src/door.c src/platform.c src/forcefield.c src/lzss.c src/archive.c src/utils.c src/strings.c src/scan.c src/hud.c src/game.c src/world.c src/animation.c src/mtx.c src/vec.c -lopengl32 -lglu32 -lfreeglut -lm
cv2pdb -C dsgraph.exe
