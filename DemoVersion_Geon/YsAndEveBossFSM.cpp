#include "YsAndEveBossFSM.h"

#include "BeatManager.h"
#include "ObjectManager.h"

#include "GameObject.h"
#include "CircleCollider.h"
#include "YsAndEveBossComponent.h"
#include "GridMoveComponent.h"
#include "GridComponent.h"

#include <random>

namespace TestGeon
{
	YsAndEveBossFSM::YsAndEveBossFSM()
		: d2dFramework::BaseEntity(50)
		, mBeatManager(nullptr)
		, mBossEve(nullptr)
		, mCurrentState(eEveBossState::Idle)
		, mNextState(eEveBossState::Idle)
		, mbIsActionTurn(false)
		, mPlayer(nullptr)
		, mDelayBeat(0)
		, mRandAttack(0)
	{
	}

	YsAndEveBossFSM::~YsAndEveBossFSM()
	{

	}

	void YsAndEveBossFSM::Init(d2dFramework::BeatManager* beatmanager)
	{
		using namespace d2dFramework;

		CircleCollider* collider = mBossEve->GetComponent<CircleCollider>();

		auto playersid = ObjectManager::GetInstance()->GetObjectTypeIDs(eObjectType::Player);

		srand(static_cast<unsigned int>(time(nullptr)));

		for (auto playerid : playersid)
		{
			mPlayer = ObjectManager::GetInstance()->FindObjectOrNull(playerid);
		}
		
		// �÷��̾�� �浹���� ��, ó�� �ݺ��Լ� ���
		auto registerOnEnterCollision = [this](unsigned int playerid)
		{
			GameObject* player = ObjectManager::GetInstance()->FindObjectOrNull(playerid);

			if (player->GetObjectType() == eObjectType::Player)
			{
				CircleCollider* collider = player->GetComponent<CircleCollider>();


			}
		};

		collider->RegisterOnEnterCollisionCallBack(mBossEve->GetId(), registerOnEnterCollision);
	}

	void YsAndEveBossFSM::Update(float deltaTime)
	{
		using namespace d2dFramework;

		if (mBossEve != nullptr)
		{
			handleState();

			if (mCurrentState != mNextState)
			{
				exit();
				mCurrentState = mNextState;
				enter();
			}
		}
	}

	void YsAndEveBossFSM::update(float deltaTime)
	{
		using namespace d2dFramework;

		static int CurrentBeat = 0;
		static int NextBeat = 0;

		YsAndEveBossComponent* BossEveComponent = mBossEve->GetComponent<YsAndEveBossComponent>();
		assert(BossEveComponent != nullptr);

		BossEveComponent->AccumulateBeatElapsedTime(deltaTime);
		NextBeat = BossEveComponent->GetAccumulateCount(mBeatManager->GetIntervalTime());

		// �׼� �ϰ� �� ���� ���� ������ ����
		if (CurrentBeat + mDelayBeat < NextBeat)
		{
			CurrentBeat = NextBeat;
			mRandAttack = std::rand() % 2;			// ���� ����

			if (mbIsActionTurn)
			{
				mbIsActionTurn = true;
			}
			else
			{
				mbIsActionTurn = false;
			}
		}

		switch (mCurrentState)
		{
		case TestGeon::eEveBossState::Idle:
			break;
		case TestGeon::eEveBossState::Trace:
			break;
		case TestGeon::eEveBossState::Attack:
			break;
		case TestGeon::eEveBossState::Death:
			break;
		}
	}

	void YsAndEveBossFSM::handleState()
	{
		using namespace d2dFramework;

		switch (mCurrentState)
		{
		case eEveBossState::Idle:
		{
			GridComponent* playergridcomponent = mPlayer->GetComponent<GridComponent>();
			GridComponent* bossgridcomponent = mBossEve->GetComponent<GridComponent>();

			if (mRandAttack == 0 && playergridcomponent->GetPosition().Y == bossgridcomponent->GetPosition().Y)
			{
				mNextState = eEveBossState::Attack;
			}
			else if (mbIsActionTurn)
			{
				mNextState = eEveBossState::Trace;
			}
		}
			break;
		case eEveBossState::Trace:						// Y��ǥ�� �Ȱ����� ����, �ٸ��� �߰�
		{
			GridComponent* playergridcomponent = mPlayer->GetComponent<GridComponent>();
			GridComponent* bossgridcomponent = mBossEve->GetComponent<GridComponent>();

			if (!(playergridcomponent->GetPosition().Y > bossgridcomponent->GetPosition().Y
				&& playergridcomponent->GetPosition().Y < bossgridcomponent->GetPosition().Y))
			{
				mNextState = eEveBossState::Attack;
			}
			else
			{
				if (!mbIsActionTurn)
				{
					mNextState = eEveBossState::Idle;
				}
			}
		}
			break;
		case TestGeon::eEveBossState::Attack:
		{
			if (!mbIsActionTurn)
			{
				mNextState = eEveBossState::Idle;
			}
		}
			break;
		case TestGeon::eEveBossState::Death:
		{
			ObjectManager::GetInstance()->DeleteObject(mBossEve->GetId());
			mBossEve = nullptr;
		}
			break;
		}
	}

	void YsAndEveBossFSM::enter()
	{
		using namespace d2dFramework;

		switch (mCurrentState)
		{
		case TestGeon::eEveBossState::Idle:			// ������
		{
		}
			break;
		case TestGeon::eEveBossState::Trace:		// y��ǥ �߰�
		{
			GridComponent* playergridcomponent = mPlayer->GetComponent<GridComponent>();
			GridComponent* bossgridcomponent = mBossEve->GetComponent<GridComponent>();

			if (playergridcomponent->GetPosition().Y > bossgridcomponent->GetPosition().Y)
			{
				GridMoveComponent* bossgridmovecomponent = mBossEve->GetComponent<GridMoveComponent>();
				bossgridmovecomponent->Move(0, 1);
			}
			else if (playergridcomponent->GetPosition().Y < bossgridcomponent->GetPosition().Y)
			{
				GridMoveComponent* bossgridmovecomponent = mBossEve->GetComponent<GridMoveComponent>();
				bossgridmovecomponent->Move(0, -1);
			}
		}
			break;
		case TestGeon::eEveBossState::Attack:
		{

			// ���� ���� ( �� �� �ϳ� ���� ? )
			// �÷��̾� �������� �� ���� �ȿ� �� ���� ����
			// �� ���ڿ� �� ĭ �� �̵��ϴ� �� 1�� �߻�
			GridComponent* playergridcomponent = mPlayer->GetComponent<GridComponent>();
			GridComponent* bossgridcomponent = mBossEve->GetComponent<GridComponent>();
			GridMoveComponent* movegridcomponent = mBossEve->GetComponent<GridMoveComponent>();

			// 0�̸� �� ���� ���� ( �̱��� )
			// 1�̸� �� ���� ���� ( �̱��� )
			if (mRandAttack == 0)
			{
				if (playergridcomponent->GetPosition().X > bossgridcomponent->GetPosition().X)
				{
					movegridcomponent->Move(MAX_X, 0);
				}
				else if (playergridcomponent->GetPosition().X < bossgridcomponent->GetPosition().X)
				{
					movegridcomponent->Move(-MAX_X, 0);
				}
			}
			else
			{

			}
		}
			break;
		case TestGeon::eEveBossState::Death:
			break;
		}
	}

	void YsAndEveBossFSM::exit()
	{
		switch (mCurrentState)
		{
		case TestGeon::eEveBossState::Idle:
			break;
		case TestGeon::eEveBossState::Trace:
			break;
		case TestGeon::eEveBossState::Attack:
			break;
		case TestGeon::eEveBossState::Death:
			break;
		}
	}

	void YsAndEveBossFSM::DebugRender(const D2D1::Matrix3x2F& camaraTransform)
	{

	}

	void YsAndEveBossFSM::Release()
	{

	}
}

