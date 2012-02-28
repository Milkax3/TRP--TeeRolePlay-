/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "mine.h"

CMine::CMine(CGameWorld *pGameWorld, int Owner, vec2 Pos,
		int Damage, int SoundImpact, int Weapon)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Pos = Pos;
	m_Owner = Owner;
	m_Damage = Damage;
	m_SoundImpact = SoundImpact;
	m_Weapon = Weapon;
	m_StartTick = Server()->Tick();

	GameWorld()->InsertEntity(this);
}

void CMine::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}


void CMine::Tick()
{
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);

	if(OwnerChar && !OwnerChar->IsAlive())
	{
		Reset();
		return;
	}

	if(((Server()->Tick() - m_StartTick) / Server()->TickSpeed()) >= 90)
	{
		GameServer()->CreateExplosion(m_Pos, m_Owner, m_Weapon, true);
		GameServer()->CreateSound(m_Pos, m_SoundImpact);
		if(OwnerChar && OwnerChar->IsAlive() && OwnerChar->GetPlayer() && OwnerChar->GetPlayer()->m_MinesCreated > 0)
			OwnerChar->GetPlayer()->m_MinesCreated --;
		Reset();
	}
	
	CCharacter *pChr = GameServer()->m_World.ClosestCharacter(m_Pos, 16.0f, 0);
	if(pChr && pChr->IsAlive() && pChr != OwnerChar)
	{
		GameServer()->CreateExplosion(m_Pos, m_Owner, m_Weapon, false);
		GameServer()->CreateSound(m_Pos, m_SoundImpact);
		if(OwnerChar && OwnerChar->IsAlive() && OwnerChar->GetPlayer() && OwnerChar->GetPlayer()->m_MinesCreated > 0)
			OwnerChar->GetPlayer()->m_MinesCreated --;
		Reset();
	}
}

void CMine::FillInfo(CNetObj_Projectile *pProj)
{
	pProj->m_X = (int)m_Pos.x;
	pProj->m_Y = (int)m_Pos.y;
	pProj->m_VelX = 0;
	pProj->m_VelY = 0;
	pProj->m_StartTick = Server()->Tick() - 1;
	pProj->m_Type = WEAPON_GRENADE;
}

void CMine::Snap(int SnappingClient)
{

	if(NetworkClipped(SnappingClient, m_Pos))
		return;

	CNetObj_Projectile *pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_ID, sizeof(CNetObj_Projectile)));
	if(pProj)
		FillInfo(pProj);
}
