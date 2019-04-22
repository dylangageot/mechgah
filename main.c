#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include "src/nes/nes.h"

#define TICK_INTERVAL 16

static uint32_t next_time;

uint32_t time_left(void)
{
    uint32_t now;

    now = SDL_GetTicks();
    if(next_time <= now)
        return 0;
    else
        return next_time - now;
}

int main(int argc, char **argv) {

  int scale_value = 2;
  int opt;

  opterr = 0; /* In order to return '?' if there is an error */

  while((opt = getopt(argc, argv, "s:")) != -1){
    switch(opt){
      case 's':
        if(isdigit(*optarg)){
          scale_value = strtol(optarg, NULL, 10);
        }else{
          fprintf (stderr,"%c is not a valid scaling value.\n", *optarg);
          return EXIT_FAILURE;
        }
        break;
      case '?':
        if (optopt == 's')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return EXIT_FAILURE;
       default:
        fprintf (stderr,"Emulator requires a ROM to run on.\n");
        return EXIT_FAILURE;
    }
  }

  if(optind == argc){
    fprintf (stderr,"Emulator requires a ROM to run on.\n");
    return EXIT_FAILURE;
  }
  char *romFileName = argv[optind];

  if(scale_value < 1 || scale_value > 15){
    fprintf(stderr, "Error: Scaling value %d is out of range.\n", scale_value);
		return EXIT_FAILURE;
  }

	NES *nes = NES_Create(romFileName);
	if (nes == NULL)
		return EXIT_FAILURE;

	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "Error: Can't initialize SDL (%s)\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Surface *ecran = SDL_SetVideoMode(256*scale_value, 240*scale_value, 32, SDL_HWSURFACE |
																SDL_DOUBLEBUF);
    SDL_WM_SetCaption("NES Emulator", NULL);

    int continuer = 1;
    SDL_Event event;
	SDL_Surface *surface = NULL, *scaled = NULL;
	SDL_Rect srcdest;
	srcdest.x = 0;
	srcdest.y = 0;

	next_time = SDL_GetTicks() + TICK_INTERVAL;
    while (continuer)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
        }

		NES_NextFrame(nes);

		PPU_RenderNametable(nes->ppu, nes->ppu->image, 0);
		PPU_RenderSprites(nes->ppu, nes->ppu->image);
		surface = SDL_CreateRGBSurfaceFrom((void*) nes->ppu->image,
					256,
					240,
					sizeof(uint32_t) * 8,			// bits per pixel = 24
					4 * 256,			// pitch
					0x00FF0000,             // red mask
					0x0000FF00,             // green mask
					0x000000FF,             // blue mask
				    0x00000000);            // alpha mask (none)
		scaled = rotozoomSurface(surface, 0, scale_value, SMOOTHING_OFF);
		SDL_FillRect(ecran, NULL, 0x000000);
		SDL_BlitSurface(scaled, NULL, ecran, &srcdest);
		SDL_Delay(time_left());
	 	SDL_Flip(ecran);
		SDL_FreeSurface(surface);
		SDL_FreeSurface(scaled);
        next_time += TICK_INTERVAL;
    }

	SDL_Quit();
	NES_Destroy(nes);
	return EXIT_SUCCESS;
}
