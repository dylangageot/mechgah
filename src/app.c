#include "app.h"
#include "common/keys.h"
#include "common/macro.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <SDL/SDL_rotozoom.h>

uint8_t App_Init(App *self, int argc, char **argv) {
	int opt;
	opterr = 0; /* In order to return '?' if there is an error */
	self->scale = 2; /* Default scaling factor is 2 */

	/* Process given option */
	while((opt = getopt(argc, argv, "s:")) != -1){
		switch(opt){
			case 's':
				if(isdigit(*optarg)){
					self->scale = strtol(optarg, NULL, 10);
				} else {
					fprintf (stderr, "%c is not a valid scaling value.\n",
							 *optarg);
					return EXIT_FAILURE;
				}
				break;
			case '?':
				if (optopt == 's')
					fprintf (stderr, "Option -%c requires an argument.\n", 
							 optopt);
				else if (isprint(optopt))
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
		fprintf (stderr, "Emulator requires a ROM to run on.\n");
		return EXIT_FAILURE;
	}
	
	if(self->scale < 1 || self->scale > 15){
		fprintf(stderr, "Error: Scaling value %d is out of range.\n", 
				self->scale);
		return EXIT_FAILURE;
	}

	if(readFileKeys("KeysConfig.txt", self->keysConfig) == 0) {
		fprintf(stderr, "Error: KeysConfig.txt is missing\n");
		return EXIT_FAILURE;
	}

	/* NES initialization */
	char *romFileName = argv[optind];
	self->nes = NES_Create(romFileName);
	if (self->nes == NULL)
		return EXIT_FAILURE;

	/* SDL initialization */
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "Error: Can't initialize SDL (%s)\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	self->screen = SDL_SetVideoMode(256 * self->scale, 240 * self->scale,
									32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Mechgah", NULL);

	return EXIT_SUCCESS;
}

uint8_t App_Execute(App *self) {
	int continuer = 1, returnValue = EXIT_SUCCESS;
	uint16_t keysPressed;
    SDL_Event event;
	SDL_Surface *surface = NULL, *scaled = NULL;
	SDL_Rect srcdest; srcdest.x = 0; srcdest.y = 0;

	self->nextFlip = SDL_GetTicks() + TICK_INTERVAL;
    while (continuer)
    {

		continuer = handleKeys(self->keysConfig, &keysPressed, &event);

		if (NES_NextFrame(self->nes, keysPressed) == EXIT_FAILURE) {
			returnValue = EXIT_FAILURE;
			continuer = 0;
		}

		surface = SDL_CreateRGBSurfaceFrom((void*) NES_Render(self->nes),
					256,
					240,
					sizeof(uint32_t) * 8,	// bits per pixel = 24
					4 * 256,				// pitch
					0x00FF0000,             // red mask
					0x0000FF00,             // green mask
					0x000000FF,             // blue mask
				    0x00000000);            // alpha mask (none)
		scaled = rotozoomSurface(surface, 0, self->scale, SMOOTHING_OFF);
		SDL_FillRect(self->screen, NULL, 0x000000);
		SDL_BlitSurface(scaled, NULL, self->screen, &srcdest);
		SDL_Delay(App_TimeLeft(self));
        self->nextFlip += TICK_INTERVAL;
	 	SDL_Flip(self->screen);
		SDL_FreeSurface(scaled);
    }

	SDL_FreeSurface(self->screen);
	SDL_Quit();
	NES_Destroy(self->nes);
	return returnValue;
}

uint32_t App_TimeLeft(App *self) {
    uint32_t now;

    now = SDL_GetTicks();
    if(self->nextFlip <= now)
        return 0;
    else
        return self->nextFlip - now;
}

