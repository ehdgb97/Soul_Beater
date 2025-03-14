#pragma once

#include "BaseEntity.h"
#include "BossState.h"

#include <d2d1helper.h>
#include <vector>
#include <set>

namespace d2dFramework
{
	class BeatManager;
	class RenderManager;
	class GameObject;
}

namespace soulBeater
{
	class MonsterFSM;

	class LilithBossFSM : public d2dFramework::BaseEntity
	{
	public:
		LilithBossFSM();
		~LilithBossFSM();

		void Init(d2dFramework::BeatManager* beatmanager, MonsterFSM* monsterFSM, unsigned int bossId);
		void Update(float deltaTime);
		void Release();

	private:
		void update(float deltaTime);
		void handleState(float deltaTime); // ���� ���º�ȭ���� �����ϰ�
		void enter(); // ���⼭ ���º�ȯ �� ���� Ʈ���� �Ǵ� ����
		void exit(); // ����� ���� ��ȯ ���� ����

		void handlePhaseState();
		void enterPhase();

		void attackPatton(int pattonNumber);

		d2dFramework::GameObject* createProjectile();

		d2dFramework::GameObject* getPlayer();


	private:
		// �÷��̾�� ����, ���� ����, ���� ����
		d2dFramework::BeatManager* mBeatManager;
		unsigned int mBossLilithId;

		eBossState mCurrentState;
		eBossState mNextState;
		ePhase mCurrentPhase;
		ePhase mNextPhase;

		MonsterFSM* mMonsterFSM;

		// �߰� ���� ���� ( ���̵� ���� )
		int mDelayBeat;
		unsigned int mBeatCount;

		// �׼� �ϰ� ���� ���� ����
		eBossTurn mBossTurn;
		bool mbIsAttackComplete;

		unsigned int mProjectileObjectId = 20005012;

		std::set<unsigned int> mProjectileIds;

		const int TURN_OF_ACTION = 1;
	};
}