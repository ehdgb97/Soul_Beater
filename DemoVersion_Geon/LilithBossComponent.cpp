#include "LilithBossComponent.h"

namespace TestGeon
{
	LilithBossComponent::LilithBossComponent(unsigned int id, d2dFramework::GameObject* owner)
		: Component::Component(id, owner)
		, mMoveableBeatCount(0)
		, mBeatElapsed(0)
	{
	}

	void LilithBossComponent::Init()
	{
	}
}