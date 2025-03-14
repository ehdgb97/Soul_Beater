#pragma once

#include "BaseEntity.h"
#include "eObjectType.h"

#include <map>
#include <cassert>
#include <unordered_map>
#include <typeinfo>

#include "ISerialize.h"

namespace d2dFramework
{
	class Component;

	class GameObject : public BaseEntity, public ISerialize
	{
		friend class ObjectManager;

	public:
		// ����� ȣ�� �� ��� ������ ��ϵǹǷ�, ��Ÿ�� ���� �ÿ��� ����� ȣ���� ���� ������. �ڵ����� ������Ʈ �Ŵ����� ���� ȣ���   
		void Init(bool bIsIndependent = false);
		// ����� ȣ�� �� ��� ������ �����ǹǷ�, ��Ÿ�� ���� �ÿ��� ����� ȣ���� ���� ������. �ڵ����� ������Ʈ �Ŵ����� ���� ȣ���
		void Release();
		void SerializeIn(nlohmann::ordered_json& object) override;
		void SerializeOut(nlohmann::ordered_json& object) override;
		template <typename T>
		T* CreateComponent(unsigned int id);
		// �θ� ���������� ����Ͽ� ������ ����ϴ� �뵵
		template <typename Base, typename Derived>
		Base* AddComponent(Derived* component);
		template <typename T>
		T* GetComponent();

		inline void SetObjectType(eObjectType objectType);  // init ȣ�� ������ ����ؾ� �մϴ�. rendering ����, collision ó�� ���� �뵵
		inline void SetParent(GameObject* gameObject); // init ȣ�� ������ ����ؾ� �մϴ�. update, fixedUpdate ó�� ���� �뵵

		inline eObjectType GetObjectType(void) const;
		inline GameObject* GetParentOrNull() const;
		inline unsigned int GetReferenceDepth() const;

	private:
		GameObject(unsigned int id, eObjectType objectType = eObjectType::None);
		~GameObject();

	public:
		enum { MAX_REFERENCE_DEPTH = 3u };

	private:
		eObjectType mObjectType;
		std::unordered_map<size_t, Component*> mComponents;
		GameObject* mParent;
		std::map<unsigned int, GameObject*> mChildren;
		unsigned int mReferenceDepth;
	};

	template <typename T>
	T* GameObject::CreateComponent(unsigned int id)
	{
		bool bIsBase = std::is_base_of<Component, T>::value;
		assert(bIsBase);

		T* component = new T(id, this);
		size_t hash = typeid(T).hash_code();
		mComponents.insert({ hash, component });

		return component;
	}

	template <typename Base, typename Derived>
	Base* GameObject::AddComponent(Derived* instance)
	{
		bool bIsBase = std::is_base_of<Component, Base>::value;
		assert(bIsBase);
		bool bIsDerived = std::is_base_of<Component, Derived>::value;
		assert(bIsDerived);
		bool bIsInheritance = std::is_base_of<Base, Derived>::value;
		assert(bIsInheritance);

		size_t hash = typeid(Base).hash_code();
		mComponents.insert({ hash, instance });

		return instance;
	}

	template <typename T>
	T* GameObject::GetComponent()
	{
		size_t hash = typeid(T).hash_code();
		auto iter = mComponents.find(hash);

		if (iter == mComponents.end())
		{
			return nullptr;
		}

		T* find = static_cast<T*>((*iter).second);

		return find;
	}

	void GameObject::SetObjectType(eObjectType objectType)
	{
		mObjectType = objectType;
	}
	void GameObject::SetParent(GameObject* gameObject)
	{
		assert(gameObject != nullptr);
		mParent = gameObject;
		mParent->mChildren.insert({ GetId(), this });
		mReferenceDepth = gameObject->GetReferenceDepth() + 1u;

		assert(MAX_REFERENCE_DEPTH > mReferenceDepth);
	}

	eObjectType GameObject::GetObjectType(void) const
	{
		return mObjectType;
	}
	GameObject* GameObject::GetParentOrNull() const
	{
		return mParent;
	}
	unsigned int GameObject::GetReferenceDepth() const
	{
		return mReferenceDepth;
	}
}
