/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "turret.h"
#include "laser.h"

CTurret::CTurret(CGameWorld *pGameWorld, vec2 Pos, vec2 Direction, float StartEnergy, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Pos = Pos;
	m_Owner = Owner;
	m_Energy = StartEnergy;
	m_Dir = Direction;
	m_Bounces = 0;
	m_EvalTick = Server()->Tick();
	GameWorld()->InsertEntity(this);
}

void CTurret::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

void CTurret::Tick()
{
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	if(OwnerChar && !OwnerChar->IsAlive())

	{
		Reset();
		return;
	}
	if((Server()->Tick() - m_EvalTick) / Server()->TickSpeed() < 10) //let the turret live for 90 seconds
	{
		if((Server()->Tick() - m_LastFire) / Server()->TickSpeed() > 0)
		{
			CCharacter *pChr = GameServer()->m_World.ClosestCharacter(m_Pos, 16.0f, 0);
			if(pChr && pChr->IsAlive() && pChr->GetPlayer()->GetCID() != m_Owner)
			{
				pChr->TakeDamage(vec2(0.f, 0.f), 15, m_Owner, WEAPON_RIFLE);
				GameServer()->CreateSound(m_Pos, SOUND_RIFLE_FIRE);
				Reset();
			}
			else
			{
				pChr = GameServer()->m_World.ClosestCharacter(m_Pos, 182.0f, 0);
				if(pChr && pChr->IsAlive() && pChr != OwnerChar && ((Server()->Tick() - pChr->GetLastSpawn()) / Server()->TickSpeed() > 1))
				{
					vec2 targetDirection = normalize(pChr->m_Pos - m_Pos);
					new CLaser(GameWorld(), m_Pos, targetDirection, GameServer()->Tuning()->m_LaserReach, m_Owner);
					GameServer()->CreateSound(m_Pos, SOUND_RIFLE_FIRE);
					m_LastFire = Server()->Tick();
				}
			}
		}
	}
	else
	{
		Reset();
		if(OwnerChar && OwnerChar->IsAlive() && OwnerChar->GetPlayer()->m_TurretsCreated > 0)
			OwnerChar->GetPlayer()->m_TurretsCreated --;
	}
}

void CTurret::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	if(!pObj)
		return;

	pObj->m_X = (int)m_Pos.x;
	pObj->m_Y = (int)m_Pos.y;
	pObj->m_FromX = (int)m_Pos.x;
	pObj->m_FromY = (int)m_Pos.y;
	pObj->m_StartTick = Server()->Tick() - 1;
}
