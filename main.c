#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include "src/nes/nes.h"

int main(int argc, char **argv) {

	if (argc == 1)
		return EXIT_FAILURE;
	
	NES *nes = NES_Create(argv[1]);
	if (nes == NULL)
		return EXIT_FAILURE;

	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "Error: Can't initialize SDL (%s)\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Surface *ecran = SDL_SetVideoMode(256*2, 240*2, 32, SDL_HWSURFACE |
																SDL_DOUBLEBUF);	
    SDL_WM_SetCaption("NES Emulator", NULL);

    int continuer = 1;
    SDL_Event event;

	int channels = 4; // for a RGB image
	unsigned int i, j;
	uint32_t *image = (uint32_t*) malloc(256 * 240 * sizeof(uint32_t));
	for (i = 0; i < 240*256; i++)
		image[i] = 0x00FFFFFF;
	SDL_Surface *surface = NULL;
	SDL_Rect srcdest;
	srcdest.x = 0;
	srcdest.y = 0;

	uint8_t context = 0x01;
	uint32_t clockPrev = 0;

    while (continuer)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
        }

		
		CPU_Execute(nes->cpu, &context, &(nes->clockCount));
		PPU_Execute(nes->ppu, &context, (nes->clockCount - clockPrev) * 3);
		clockPrev = nes->clockCount;
		if (context == 0x02) {
			PPU_RenderNametable(nes->ppu, image, 0);
			SDL_FillRect(ecran, NULL, 0x000000);
			surface = SDL_CreateRGBSurfaceFrom((void*) image,
					256,
					240,
					sizeof(uint32_t) * 8,			// bits per pixel = 24
					4 * 256,			// pitch
					0x00FF0000,             // red mask
					0x0000FF00,             // green mask
					0x000000FF,             // blue mask
				    0x00000000);            // alpha mask (none)
			surface = rotozoomSurface(surface, 0, 2, SMOOTHING_OFF);  
			SDL_BlitSurface(surface, NULL, ecran, &srcdest);
			SDL_Flip(ecran);
			SDL_Delay(5);
		}

    }

	SDL_Quit();
	free(image);
	return EXIT_SUCCESS;
}
