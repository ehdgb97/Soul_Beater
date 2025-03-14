#pragma once

#include <unordered_set>

namespace d2dFramework
{
	class BaseEntity abstract
	{
	public:
		BaseEntity(unsigned int id);
		virtual ~BaseEntity() = 0;

		inline unsigned int GetId() const;

		// ������ ����� ȣ�� ���� ����
		static void ClearIdMap()
		{
			mIDSet.clear();
		}

	private:
		static std::unordered_set<unsigned int> mIDSet;

		unsigned int mId;
		// �α� ���� �Ÿ� ��ü ���� ���� �ð� ���� �߰��� �� ���� �� ����.
	};

	unsigned int BaseEntity::GetId() const
	{
		return mId;
	}
}