#pragma once

#include "BaseEntity.h"
#include "BossState.h"

#include <d2d1helper.h>
#include <vector>

namespace d2dFramework
{
	class BeatManager;
	class RenderManager;
	class GameObject;
}

namespace soulBeater
{
	class MonsterFSM;

	class EveBossFSM : public d2dFramework::BaseEntity
	{
	public:
		EveBossFSM();
		~EveBossFSM();

		void Init(d2dFramework::BeatManager* beatmanager, MonsterFSM* monsterFSM, unsigned int ysID, unsigned int eveID, unsigned int playerID);
		void Update(float deltaTime);
		void DebugRender(const D2D1::Matrix3x2F& camaraTransform);
		void Release();

	private:
		void update(float deltaTime);
		void handleState(float deltaTime); // ���� ���º�ȭ���� �����ϰ�
		void enter(); // ���⼭ ���º�ȯ �� ���� Ʈ���� �Ǵ� ����
		void exit(); // ����� ���� ��ȯ ���� ����

		void handlePhaseState();
		void enterPhase();

		void attackPatton();

	private:
		// �÷��̾�� ����, ���� ����, ���� ����
		d2dFramework::BeatManager* mBeatManager;
		MonsterFSM* mMonsterFSM;
		unsigned int mYsID;
		unsigned int mEveID;
		unsigned int mPlayerID;

		unsigned int mStartBeat;

		eBossState mCurrentState;
		eBossState mNextState;
		ePhase mCurrentPhase;
		ePhase mNextPhase;

		// �߰� ���� ���� ( ���̵� ���� )
		int mDelayBeat;

		// �׼� �ϰ� ���� ���� ����
		eBossTurn mBossTurn;
		bool mbIsAttackComplete;
		int mRandAttack;

		unsigned int mPrevPlayerY;

		const int TURN_OF_ACTION = 1;
	};
}