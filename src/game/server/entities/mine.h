/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_MINE_H
#define GAME_SERVER_ENTITIES_MINE_H

class CMine : public CEntity
{
public:
	CMine(CGameWorld *pGameWorld, int Owner, vec2 Pos,
		int Damage, int SoundImpact, int Weapon);

	vec2 GetPos(float Time);
	void FillInfo(CNetObj_Projectile *pProj);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

private:
	int m_Owner;
	int m_Type;
	int m_Damage;
	int m_SoundImpact;
	int m_Weapon;
	int m_StartTick;
};

#endif
