#include "Input.h"
#include "Entity.h"
#include "Render.h"

bool InputUpdate() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			return false;
		}

		if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
			if (e.key.keysym.scancode == SDL_SCANCODE_W) {
				INPUT_FORWARD = e.type == SDL_KEYDOWN;
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_A) {
				INPUT_LEFT = e.type == SDL_KEYDOWN;
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_S) {
				INPUT_BACKWARD = e.type == SDL_KEYDOWN;
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_D) {
				INPUT_RIGHT = e.type == SDL_KEYDOWN;
			}
		}
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				INPUT_LOCKMOUSE = !INPUT_LOCKMOUSE;
				SDL_SetRelativeMouseMode(INPUT_LOCKMOUSE ? SDL_TRUE : SDL_FALSE);

				if (!INPUT_LOCKMOUSE)
					SDL_WarpMouseInWindow(RENDER_WINDOW, RENDER_WIDTH / 2, RENDER_HEIGHT / 2);
			}
		}

		if (e.type == SDL_MOUSEMOTION) {
			if (INPUT_LOCKMOUSE) {
				LOCALPLAYER.setYawAngles(LOCALPLAYER.getYawAngles() + ((double)e.motion.xrel / 100.0));
			}
		}
	}
	return true;
}
