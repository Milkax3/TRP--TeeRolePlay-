/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_TURRET_H
#define GAME_SERVER_ENTITIES_TURRET_H

#include <game/server/entity.h>

class CTurret : public CEntity
{
public:
	CTurret(CGameWorld *pGameWorld, vec2 Pos, vec2 Direction, float StartEnergy, int Owner);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

protected:
	bool HitCharacter(vec2 From, vec2 To);

private:
	vec2 m_From;
	vec2 m_Dir;
	float m_Energy;
	int m_Bounces;
	int m_EvalTick;
	int m_LastFire;
	int m_Owner;
};

#endif
