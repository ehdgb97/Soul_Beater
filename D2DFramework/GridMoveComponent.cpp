#include "GridMoveComponent.h"

#include "Transform.h"
#include "GridComponent.h"

namespace d2dFramework
{
	GridMoveComponent::GridMoveComponent(unsigned int id, GameObject* owner)
		: Component(id, owner)
		, mOwnerGridComponent(nullptr)
		, mOwnerTransform(nullptr)
		, mObjectSpeed(0.f)
		, mSpeedRatio(0.f)
		, mDelayTime(0.f)
		, mCurrentGridPosition()
		, mNextGirdPosition()
	{
	}
	void GridMoveComponent::Init()
	{
		IUpdateable::Init();

		mOwnerGridComponent = Component::GetGameObject()->GetComponent<GridComponent>();
		mOwnerTransform = Component::GetGameObject()->GetComponent<Transform>();
		mCurrentGridPosition = (mOwnerGridComponent->GetPosition());
		mNextGirdPosition = (mOwnerGridComponent->GetPosition());
	};

	void GridMoveComponent::Release()
	{
		IUpdateable::Release();

		mOwnerGridComponent = nullptr;
		mOwnerTransform = nullptr;
	}

	void GridMoveComponent::Update(float deltaTime)
	{
		// ���� : ȫ��ȯ �׸��� �����Ʈ ������Ʈ update�� �̵� ���� ó���� �ǹ��ϹǷ� ���� ��ġ�� ������ġ�� ���ٸ� ������Ʈ ���� ��ü�� �ʿ� ����
		if (mCurrentGridPosition == mNextGirdPosition)
		{
			return;
		}
		if (mDelayTime >= 1.f)
		{
			mCurrentGridPosition = mNextGirdPosition;
			mOwnerGridComponent->SetPosition(mCurrentGridPosition);

			return; // early exit
		}

		mDelayTime = mDelayTime + deltaTime * mSpeedRatio;

		float currentXPos = static_cast<float>(mCurrentGridPosition.X * mOwnerGridComponent->GetCellDistance());
		float NextXPos = static_cast<float>(mNextGirdPosition.X * mOwnerGridComponent->GetCellDistance());
		float currentYPos = static_cast<float>(mCurrentGridPosition.Y * mOwnerGridComponent->GetCellDistance());
		float NextYPos = static_cast<float>(mNextGirdPosition.Y * mOwnerGridComponent->GetCellDistance());

		//Percent
		mOwnerTransform->SetTranslate(
			{ MathHelper::Interpolation(currentXPos, NextXPos, mDelayTime * 2)
			, MathHelper::Interpolation(currentYPos, NextYPos, mDelayTime * 2) });
	}

	bool GridMoveComponent::Move(int x, int y)
	{
		// ���� : ȫ��ȯ, ���� ��ȯ�� else�� ������ ������ if������ ���� �� �� ������, �ִ밪 �̵� �õ��� ������ ��ȯ�ϵ��� ����
		if (mCurrentGridPosition.X == 0 && x < 0)
		{
			return false;
		}
		if (mCurrentGridPosition.X == mOwnerGridComponent->GetMaxX() - 1 && x > 0)
		{
			return false;
		}
		if (mCurrentGridPosition.Y == 0 && y < 0)
		{
			return false;
		}
		if (mCurrentGridPosition.Y == mOwnerGridComponent->GetMaxY() - 1 && y > 0)
		{
			return false;
		}

		mNextGirdPosition.X = mCurrentGridPosition.X + x;
		mNextGirdPosition.Y = mCurrentGridPosition.Y + y;
		// ���� ȫ��ȯ : �ӵ� ������ �ӵ��� ����Ǿ��� �� ���� ó���Ǵ� �� �ùٸ�
		// mSpeedRatio = 1.f / mObjectSpeed;
		mDelayTime = 0.f;

		// �߰� �̰��� : ���� ��ġ�� 0���� �۰ų� MaxX���� ũ�� �ּҰ� �� �ִ밪���� ����
		// unsinged int�� 0���� �۾����� ��쿡 ���� unsigned int�� �ִ� ���� ��������� ������ ���ڷ� 1000000�� ����
		if (mNextGirdPosition.X > 1000000 || mNextGirdPosition.X <= 0)
		{
			mNextGirdPosition.X = 1;
			return false;
		}
		if (mNextGirdPosition.Y > 1000000 || mNextGirdPosition.Y <= 0)
		{
			mNextGirdPosition.Y = 1;
			return false;
		}
		if (mNextGirdPosition.X > mOwnerGridComponent->GetMaxX())
		{
			mNextGirdPosition.X = mOwnerGridComponent->GetMaxX() - 1;
			return false;
		}
		if (mNextGirdPosition.Y > mOwnerGridComponent->GetMaxY())
		{
			mNextGirdPosition.Y = mOwnerGridComponent->GetMaxY() - 1;
			return false;
		}

		return true;
	}

	void GridMoveComponent::MoveNextPositon(const GridPosition& nextPosition)
	{
		mNextGirdPosition = nextPosition;
		mDelayTime = 0.f;

		if (mNextGirdPosition.X >= mOwnerGridComponent->GetMaxX() - 1)
		{
			mNextGirdPosition.X = mOwnerGridComponent->GetMaxX() - 1;
		}
		if (mNextGirdPosition.Y >= mOwnerGridComponent->GetMaxY() - 1)
		{
			mNextGirdPosition.Y = mOwnerGridComponent->GetMaxY() - 1;
		}
	}

	inline void GridMoveComponent::SerializeIn(nlohmann::ordered_json& object)
	{
	}

	inline void GridMoveComponent::SerializeOut(nlohmann::ordered_json& object)
	{
	}
}