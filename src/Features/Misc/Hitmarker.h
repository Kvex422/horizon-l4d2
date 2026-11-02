#pragma once

#include "../../SDK/SDK.h"
#include "../../Features/Vars.h"

class IGameEvent
{
public:
	virtual ~IGameEvent() {};
	virtual const char* GetName() const = 0;
	virtual bool IsReliable() const = 0;
	virtual bool IsLocal() const = 0;
	virtual bool IsEmpty(const char* keyName = nullptr) = 0;
	virtual bool GetBool(const char* keyName = nullptr, bool defaultValue = false) = 0;
	virtual int GetInt(const char* keyName = nullptr, int defaultValue = 0) = 0;
	virtual float GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) = 0;
	virtual const char* GetString(const char* keyName = nullptr, const char* defaultValue = "") = 0;
};

class IGameEventListener2
{
public:
	virtual ~IGameEventListener2() {}
	virtual void FireGameEvent(IGameEvent* event) = 0;
	virtual int GetEventDebugID() { return 42; }
};

class IGameEventManager2
{
public:
	virtual ~IGameEventManager2() {};
	virtual int LoadEventsFromFile(const char* filename) = 0;
	virtual void Reset() = 0;
	virtual bool AddListener(IGameEventListener2* listener, const char* name, bool bServerSide) = 0;
	virtual bool FindListener(IGameEventListener2* listener, const char* name) = 0;
	virtual void RemoveListener(IGameEventListener2* listener) = 0;
};

class CHitmarker : public IGameEventListener2
{
public:
	struct HitInfo_t
	{
		float m_flTime;
		float m_flAlpha;
	};

	inline void Init()
	{
		auto* pEventMgr = U::Interface.Get<IGameEventManager2*>("engine.dll", "GAMEEVENTSMANAGER002");

		if (pEventMgr)
		{
			pEventMgr->AddListener(this, "player_hurt", false);
			pEventMgr->AddListener(this, "infected_hurt", false);
			pEventMgr->AddListener(this, "infected_death", false);
			m_pEventMgr = pEventMgr;
		}
	}

	void FireGameEvent(IGameEvent* event) override
	{
		if (!Vars::Misc::bHitmarker)
			return;

		if (!I::EngineClient->IsInGame())
			return;

		const char* szName = event->GetName();

		if (!strcmp(szName, "player_hurt"))
		{
			int nAttacker = I::EngineClient->GetPlayerForUserID(event->GetInt("attacker"));
			int nLocal = I::EngineClient->GetLocalPlayer();

			if (nAttacker == nLocal)
			{
				m_Hits.push_back({ I::GlobalVars->curtime, 1.0f });

				if (Vars::Misc::bHitSound)
					I::VGuiSurface->PlaySound("buttons/arena_switch_press_02.wav");
			}
		}
		else if (!strcmp(szName, "infected_hurt") || !strcmp(szName, "infected_death"))
		{
			int nAttacker = event->GetInt("attacker");
			int nLocal = I::EngineClient->GetLocalPlayer();

			if (nAttacker == nLocal)
			{
				m_Hits.push_back({ I::GlobalVars->curtime, 1.0f });

				if (Vars::Misc::bHitSound)
					I::VGuiSurface->PlaySound("buttons/arena_switch_press_02.wav");
			}
		}
	}

	inline void Render()
	{
		if (!Vars::Misc::bHitmarker)
			return;

		int nCenterX = G::Draw.m_nScreenW / 2;
		int nCenterY = G::Draw.m_nScreenH / 2;

		for (auto it = m_Hits.begin(); it != m_Hits.end();)
		{
			float flDelta = I::GlobalVars->curtime - it->m_flTime;

			if (flDelta > 0.5f)
			{
				it = m_Hits.erase(it);
				continue;
			}

			it->m_flAlpha = 1.0f - (flDelta / 0.5f);
			byte alpha = static_cast<byte>(it->m_flAlpha * 255.0f);

			int nSize = 8;
			Color clr = { 255, 255, 255, alpha };

			G::Draw.Line(nCenterX - nSize, nCenterY - nSize, nCenterX - (nSize / 3), nCenterY - (nSize / 3), clr);
			G::Draw.Line(nCenterX + nSize, nCenterY - nSize, nCenterX + (nSize / 3), nCenterY - (nSize / 3), clr);
			G::Draw.Line(nCenterX - nSize, nCenterY + nSize, nCenterX - (nSize / 3), nCenterY + (nSize / 3), clr);
			G::Draw.Line(nCenterX + nSize, nCenterY + nSize, nCenterX + (nSize / 3), nCenterY + (nSize / 3), clr);

			++it;
		}
	}

	inline void Shutdown()
	{
		if (m_pEventMgr)
			m_pEventMgr->RemoveListener(this);
	}

private:
	std::vector<HitInfo_t> m_Hits;
	IGameEventManager2* m_pEventMgr = nullptr;
};

namespace F { inline CHitmarker Hitmarker; }
