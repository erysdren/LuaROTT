
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

static SDLNet_DatagramSocket *socket = NULL;
static Uint16 port = 34858;

static void log_func(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
	Uint64 when;
	double seconds;
	static const char *priorities[SDL_LOG_PRIORITY_COUNT] = {
		[SDL_LOG_PRIORITY_VERBOSE] = "VERBOSE: ",
		[SDL_LOG_PRIORITY_DEBUG] = "DEBUG: ",
		[SDL_LOG_PRIORITY_INFO] = "INFO: ",
		[SDL_LOG_PRIORITY_WARN] = "WARNING: ",
		[SDL_LOG_PRIORITY_ERROR] = "ERROR: ",
		[SDL_LOG_PRIORITY_CRITICAL] = "CRITICAL: "
	};

	if (priority < SDL_GetLogPriority(category))
		return;

	when = SDL_GetTicks();
	seconds = (double)when / 1000.0f;

	fprintf(stderr, "[%09.4f] %s%s\n", seconds, priorities[priority], message);
}

static int check_arg(int argc, char **argv, const char *arg)
{
	for (int i = 0; i < argc; i++)
	{
		/* check if arg is there */
		if (SDL_strcmp(argv[i], arg) == 0)
			return 1;
	}

	return 0;
}

static const char *get_arg(int argc, char **argv, const char *arg)
{
	for (int i = 0; i < argc; i++)
	{
		/* check if arg is there */
		if (SDL_strcmp(argv[i], arg) != 0)
			continue;

		/* check if it has a value */
		if (argc > (i + 1))
			return argv[i + 1];
	}

	return NULL;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
	const char *arg;

	/* set log func */
	SDL_SetLogOutputFunction(log_func, NULL);

	if (!SDL_Init(SDL_INIT_EVENTS))
	{
		SDL_Log("SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_Log("SDL: Initialized");

	if (!SDLNet_Init())
	{
		SDL_Log("SDLNet: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_Log("SDLNet: Initialized");

	/* process args */
	if ((arg = get_arg(argc, argv, "--port")) != NULL)
		port = SDL_atoi(arg);

	/* create socket */
	socket = SDLNet_CreateDatagramSocket(NULL, port);
	if (!socket)
	{
		SDL_Log("SDLNet: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_Log("SDLNet: Listening on port %d", port);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	SDLNet_DestroyDatagramSocket(socket);
	SDLNet_Quit();
	SDL_Quit();
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	SDLNet_Datagram *datagram = NULL;

	while ((SDLNet_ReceiveDatagram(socket, &datagram) == true) && (datagram != NULL))
	{
		SDL_Log("Got %d-byte datagram from %s:%d", datagram->buflen, SDLNet_GetAddressString(datagram->addr), datagram->port);
		SDLNet_DestroyDatagram(datagram);
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	if (event->type == SDL_EVENT_QUIT)
		return SDL_APP_SUCCESS;

	return SDL_APP_CONTINUE;
}
