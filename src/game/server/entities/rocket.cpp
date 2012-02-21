/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "rocket.h"

CRocket::CRocket(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, int Span,
		int Damage, bool Explosive, float Force, int SoundImpact, int Weapon)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Pos = Pos;
	m_Direction = Dir;
	m_LifeSpan = Span;
	m_Owner = Owner;
	m_Force = Force;
	m_Damage = Damage;
	m_SoundImpact = SoundImpact;
	m_Weapon = Weapon;
	m_StartTick = Server()->Tick();
	m_Explosive = Explosive;
	m_SpeedScaler = 0.9f;

	GameWorld()->InsertEntity(this);
}

void CRocket::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

vec2 CRocket::GetPos(float Time)
{
	float Curvature = 0;
	float Speed = 0;
	
	Curvature = 0;
	Speed = (GameServer()->Tuning()->m_GrenadeSpeed * m_SpeedScaler);
	
	vec2 tmpDir = m_Direction;
	return CalcPos(m_Pos, tmpDir, Curvature, Speed, Time);
}


void CRocket::Tick()
{
	float Pt = (Server()->Tick()-m_StartTick-1)/(float)Server()->TickSpeed();
	float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();
	vec2 PrevPos = GetPos(Pt);
	vec2 CurPos = GetPos(Ct);
	
	float Speed = (GameServer()->Tuning()->m_GrenadeSpeed * m_SpeedScaler);
	m_PrivatePos.x = m_Pos.x + m_Direction.x*Pt*Speed;
	m_PrivatePos.y = m_Pos.y + m_Direction.y*Pt*Speed;

	if(m_LastTick < (Server()->Tick() + Server()->TickSpeed() / 2))
	{
		GameServer()->CreateExplosion(m_PrivatePos, m_Owner, WEAPON_GRENADE, true);
		m_LastTick = Server()->Tick();
	}
	
	int Collide = GameServer()->Collision()->IntersectLine(PrevPos, CurPos, &CurPos, 0);
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	CCharacter *TargetChr = GameServer()->m_World.IntersectCharacter(PrevPos, CurPos, 6.0f, CurPos, OwnerChar);

	m_LifeSpan--;

	if(TargetChr || Collide || m_LifeSpan < 0 || GameLayerClipped(CurPos))
	{
		if(m_LifeSpan >= 0 || m_Weapon == WEAPON_GRENADE)
			GameServer()->CreateSound(CurPos, m_SoundImpact);

		if(m_Explosive)
			GameServer()->CreateExplosion(CurPos, m_Owner, m_Weapon, false);
 
		else if(TargetChr)
			TargetChr->TakeDamage(m_Direction * max(0.001f, m_Force), m_Damage, m_Owner, m_Weapon);

		GameServer()->m_World.DestroyEntity(this);
	}
}

void CRocket::FillInfo(CNetObj_Projectile *pProj)
{
	pProj->m_X = (int)m_PrivatePos.x;
	pProj->m_Y = (int)m_PrivatePos.y;
	pProj->m_VelX = 0;
	pProj->m_VelY = 0;
	pProj->m_StartTick = Server()->Tick() - 1;
	pProj->m_Type = WEAPON_GRENADE;
}

void CRocket::Snap(int SnappingClient)
{
	float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();

	if(NetworkClipped(SnappingClient, GetPos(Ct)))
		return;

	CNetObj_Projectile *pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_ID, sizeof(CNetObj_Projectile)));
	if(pProj)
		FillInfo(pProj);
}
