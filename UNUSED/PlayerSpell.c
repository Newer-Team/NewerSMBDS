#include "game.h"
#include "registers.h"
#include "sfx.h"
#include "PlayerSpell.h"
#include "Files.h"

int SpellsInPlay = 0;

u32 PlayerSpell_Collisions[] =
{
	0x00000000,
	0x15554000,
	0x15A5A440,
	0x00055000,
	0x00181500,
	0x50550544,
	0x41109561,
	0x50419409,
	0x40010A9A,
	0x02441555,
	0x00000000,
	0x00080000,
	0x00000000,
	0x00004000,
	0xA0000000,
	0x40000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
};

int PlayerSpell_CollisionCode(ActivePhysics* Spell, ActivePhysics* Object)
{
	EnemyClassObject* ObjectActor = (EnemyClassObject*)Object->owner;
	PlayerSpell* SpellActor = Spell->owner;

	u16 actorID = ObjectActor->ClassID;
	u8 collType = PlayerSpell_Collisions[actorID / 16] >> (actorID % 16 * 2) & 3;

	if (collType == 0)
		return 0;
	
	SpellActor->InPlay = false;

	if (SpellsInPlay > 0)
		SpellsInPlay--;

	if (collType == 1)
	{
		UnregisterActivePhysics(Object);
		Vec3 ScorePos = ObjectActor->position;
		ScorePos.y += 0x18000;

		AddPointsToScore(0, 200);
		DrawScore(1, &ScorePos);
		
		SpawnParticle(0x41, &ObjectActor->position);
		PlaySNDEffect(SE_OBJ_FIREBALL_DISAPP, &ObjectActor->position);	
		createActor(66, 5, &ObjectActor->position, 0, 0, 0);
		Base__deleteIt(ObjectActor);
	}
	else
		PlaySNDEffect(SE_OBJ_FIREBALL_DISAPP, &ObjectActor->position);
	
	SpellActor->state = 1;

	return 0;
}

void PlayerSpell_setup(PlayerSpell* spell)
{
	int Tex = RNG(3);
	u8* texture = (u8*)NNS_G3dGetTex((u32*)getPtrToLoadedFile(PLAYER_SPELL));
	getTextureParams(texture, Tex, &spell->Texture.texparam);
	getTexturePalBase(texture, Tex, &spell->Texture.palbase);

	spell->RotateTimer = 0;

	u32 AP_PlayerSpell[] = 
	{
		0x8000,// X offset
		0x8000,// Y offset
		0x8000,// Width
		0x8000,// Height
		0x09300001, // behavior
		0x0000,// flags
		(u32)&PlayerSpell_CollisionCode
	};

	spell->DrawDist.x = 0x20;
	spell->DrawDist.y = 0x20;
	spell->ExistDist.x = 0x20;
	spell->ExistDist.y = 0x20;
	spell->rotation.z = 0;
	spell->scale.x = 0x12000;
	spell->scale.y = 0x12000;
	spell->scale.z = 0x12000;

	initActivePhysics(&spell->activephysics, spell, AP_PlayerSpell, 0);
	registerCollider(&spell->activephysics);
	
	SpellsInPlay++;
	spell->InPlay = true;
}

void PlayerSpell_AfterDelete(PlayerSpell* spell)
{
	if (SpellsInPlay && spell->InPlay) 
		SpellsInPlay--;	
	
	actor_afterDelete(spell);
}

void PlayerSpell_State0(PlayerSpell* spell)
{
	spell->rotation.z += 0x400;
	spell->position.x += spell->velocity.x;
	spell->position.y += spell->velocity.y;

	Vec3 ParticlePosition = spell->position;
	ParticlePosition.x += spell->scale.x / 2;
	ParticlePosition.y += spell->scale.y / 2;
	
	SpawnAndEndParticle(40, 101, &ParticlePosition, 0, 0, 0, 0);
	
	DeleteIfOutOfRange(spell, 0);
	return;
}

void PlayerSpell_State1(PlayerSpell* spell)
{
	if (spell->scale.x > 0)
	{
		spell->scale.x -= 0x1000;
		spell->scale.y -= 0x1000;
	}
	else
		Base__deleteIt(spell);
	return;
}

void PlayerSpell_DieOnFlagpole(PlayerSpell* spell)
{
	Base__deleteIt(spell);
}

bool PlayerSpell_BeforeDraw()
{
	return 1;
}

static inline void vec3(int x, int y, int z)
{
	REG_VERTEX16 = (y << 16) | (x & 0xFFFF);
	REG_VERTEX16 = (z & 0xFFFF);
}

static inline void texcoord2(int x, int y)
{
	REG_TEXT_COORD = (y << 16) | (x & 0xFFFF);
}

void G3_LoadMtx43(u32 unk);

void PlayerSpell_Draw(PlayerSpell* spell)
{
	NNS_G3dGeFlushBuffer();
	G3_LoadMtx43(0x02085B20);

	if ((*(*(int**)(0x208B168)) >> 20) & 0xF) REG_MTX_TRANSLATE = (spell->position.x & 0x000FFFFF);
	else REG_MTX_TRANSLATE = spell->position.x;
	
	REG_MTX_TRANSLATE = spell->position.y;
	REG_MTX_TRANSLATE = spell->position.z;

	glTranslatef32(0x8000,0x8000,0x8000);
	glRotateZi(spell->rotation.z);
	glTranslatef32(-0x8000,-0x8000,-0x8000);

	REG_MTX_SCALE = spell->scale.x;
	REG_MTX_SCALE = spell->scale.y;
	REG_MTX_SCALE = spell->scale.z;
	 
	REG_MTX_CONTROL = 3;
	REG_MTX_IDENTITY = 0;
	REG_MTX_CONTROL = 2;

	REG_POLY_FORMAT = 0x001F3880;
	REG_TEXT_FORMAT = spell->Texture.texparam;
	REG_PAL_BASE = spell->Texture.palbase;

	REG_COLOR= 0x7FFF;

	REG_GFX_BEGIN = 1;
	texcoord2(0x100, 0);
	vec3(0x1000, 0x1000, 0x0);

	texcoord2(0, 0);
	vec3(0x0000,0x1000, 0x0);

	texcoord2(0, 0x100);
	vec3(0x0000, 0x0000, 0x0);

	texcoord2(0x100, 0x100);
	vec3(0x1000, 0x0000, 0x0);

	REG_GFX_END = 0;
}