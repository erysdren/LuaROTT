#pragma author erysdren
#pragma description Rise of the Triad Demo Format
#pragma endian little

#include <std/mem.pat>

enum Product : u32 {
	SHAREWARE,
	REGISTERED,
	SUPERCD,
	SITELICENSE
};

struct BattleType {
	u32 Gravity;
	u32 Speed;
	u32 Ammo;
	u32 HitPos32s;
	u32 SpawnDangers;
	u32 SpawnHealth;
	u32 SpawnWeapons;
	u32 SpawnMines;
	u32 RespawnItems;
	u32 WeaponPersistence;
	u32 RandomWeapons;
	u32 FriendlyFire;
	u32 LightLevel;
	s32 Kills;
	s32 DangerDamage;
	u32 TimeLimit;
	u32 RespawnTime;
};

struct Specials {
	s32 GodModeTime;
	s32 DogModeTime;
	s32 ShroomsModeTime;
	s32 ElastoModeTime;
	s32 AsbestosVestTime;
	s32 BulletProofVestTime;
	s32 GasMaskTime;
	s32 MercuryModeTime;
	s32 GodModeRespawnTime;
	s32 DogModeRespawnTime;
	s32 ShroomsModeRespawnTime;
	s32 ElastoModeRespawnTime;
	s32 AsbestosVestRespawnTime;
	s32 BulletProofVestRespawnTime;
	s32 GasMaskRespawnTime;
	s32 MercuryModeRespawnTime;
};

struct GameType {
	u32 Version;
	Product Product;
	s32 TimeCount;
	s32 frame;
	s32 secrettotal;
	s32 treasuretotal;
	s32 killtotal;
	s32 secretcount;
	s32 treasurecount;
	s32 killcount;
	s32 supertotal;
	s32 healthtotal;
	s32 missiletotal;
	s32 supercount;
	s32 healthcount;
	s32 missilecount;
	s32 democratictotal;
	s32 planttotal;
	s32 democraticcount;
	s32 plantcount;
	s32 dipballs;
	s32 difficulty;
	s32 violence;
	s32 mapon;
	s32 score;
	s32 episode;
	s32 battlemode;
	s32 battleoption;
	s32 randomseed;
	u8 teamplay;
	u8 DODEMOCRATICBONUS1;
	u8 DOGROUNDZEROBONUS;
	s32 autorun;
	BattleType BattleOptions;
	u8 SpawnCollectItems;
	u8 SpawnEluder;
	u8 SpawnDeluder;
	u8 ShowScores;
	if (Product == Product::SHAREWARE) {
		u8 PlayerHasGun[5];
	} else {
		u8 PlayerHasGun[11];
	}
	Specials SpecialsTimes;
};

struct DemoPacket {
	s32 time;
	s16 momx;
	s16 momy;
	u16 dangle;
	u16 buttons;
};

GameType game_type @ 0x00;
DemoPacket packets[while($ < std::mem::size() - sizeof(DemoPacket))] @ $;
