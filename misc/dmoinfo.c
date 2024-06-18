
#include <SDL3/SDL.h>

typedef struct battle_type {
	Uint32 Gravity;
	Uint32 Speed;
	Uint32 Ammo;
	Uint32 HitPoSint32s;
	Uint32 SpawnDangers;
	Uint32 SpawnHealth;
	Uint32 SpawnWeapons;
	Uint32 SpawnMines;
	Uint32 RespawnItems;
	Uint32 WeaponPersistence;
	Uint32 RandomWeapons;
	Uint32 FriendlyFire;
	Uint32 LightLevel;
	Sint32 Kills;
	Sint32 DangerDamage;
	Uint32 TimeLimit;
	Uint32 RespawnTime;
} battle_type_t;

typedef struct specials {
	Sint32 GodModeTime;
	Sint32 DogModeTime;
	Sint32 ShroomsModeTime;
	Sint32 ElastoModeTime;
	Sint32 AsbestosVestTime;
	Sint32 BulletProofVestTime;
	Sint32 GasMaskTime;
	Sint32 MercuryModeTime;
	Sint32 GodModeRespawnTime;
	Sint32 DogModeRespawnTime;
	Sint32 ShroomsModeRespawnTime;
	Sint32 ElastoModeRespawnTime;
	Sint32 AsbestosVestRespawnTime;
	Sint32 BulletProofVestRespawnTime;
	Sint32 GasMaskRespawnTime;
	Sint32 MercuryModeRespawnTime;
} specials_t;

typedef struct game_type {
	Uint32 Version;
	Uint32 Product;
	Sint32 TimeCount;
	Sint32 frame;
	Sint32 secrettotal;
	Sint32 treasuretotal;
	Sint32 killtotal;
	Sint32 secretcount;
	Sint32 treasurecount;
	Sint32 killcount;
	Sint32 supertotal;
	Sint32 healthtotal;
	Sint32 missiletotal;
	Sint32 supercount;
	Sint32 healthcount;
	Sint32 missilecount;
	Sint32 democratictotal;
	Sint32 planttotal;
	Sint32 democraticcount;
	Sint32 plantcount;
	Sint32 dipballs;
	Sint32 difficulty;
	Sint32 violence;
	Sint32 mapon;
	Sint32 score;
	Sint32 episode;
	Sint32 battlemode;
	Sint32 battleoption;
	Sint32 randomseed;
	Uint8 teamplay;
	Uint8 DODEMOCRATICBONUS1;
	Uint8 DOGROUNDZEROBONUS;
	Sint32 autorun;
	battle_type_t BattleOptions;
	Uint8 SpawnCollectItems;
	Uint8 SpawnEluder;
	Uint8 SpawnDeluder;
	Uint8 ShowScores;
	Uint8 PlayerHasGun[11];
	specials_t SpecialsTimes;
} game_type_t;

typedef struct demo_packet {
	Sint32 time;
	Sint16 momx;
	Sint16 momy;
	Uint16 dangle;
	Uint16 buttons;
} demo_packet_t;

int main(int argc, char **argv)
{
	if (argc != 2)
		return 1;

	size_t dmosize;
	void *dmo = SDL_LoadFile(argv[1], &dmosize);
	if (!dmo) return 1;

	game_type_t *game = (game_type_t *)dmo;

	SDL_Log("Processing \"%s\"\n", argv[1]);
	SDL_Log("Version: %d\n", game->Version);
	SDL_Log("Product: %d\n", game->Product);
	SDL_Log("TimeCount: %d\n", game->TimeCount);
	SDL_Log("Map: E%dA%d\n", game->episode, game->mapon);

	demo_packet_t *packet = (demo_packet_t *)((Uint8 *)dmo + sizeof(game_type_t));
	int i = 0;
	while ((Uint8 *)packet < (Uint8 *)dmo + dmosize - sizeof(demo_packet_t))
	{
		SDL_Log("%d: time=%0.4f\n", i, (float)packet->time / 35.0f);
		i++;
		packet++;
	}

	SDL_free(dmo);

	return 0;
}
