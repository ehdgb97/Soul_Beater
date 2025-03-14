#include "Scene.h"

#include "GameObject.h"
#include "ICollideable.h"
#include "IFixedUpdateable.h"
#include "IUpdateable.h"
#include "IRenderable.h"
#include "ObjectManager.h"
#include "RenderManger.h"
#include "SceneLoader.h"
#include "EventManager.h"

namespace d2dFramework
{
	Scene::Scene(unsigned int id)
		: BaseEntity(id)
	{
	}

	void Scene::Enter()
	{
		SceneLoader::LoadScene(this);
	}

	void Scene::Exit()
	{
		// ������Ʈ�� ���� ť�� ���
		ObjectManager::GetInstance()->ClearObjects(false);
		// ���� ť�� ��ϵ� ������Ʈ ��� ����
		ObjectManager::GetInstance()->handleObjectLifeSpan();
		// �������� Ÿ�� ���̵� �ߺ��Ǿ �ش� ������� ó��, �ùٸ� ����� �ƴ�
	}

	void Scene::SerializeIn(nlohmann::ordered_json& object)
	{
		for (auto& jsonGameObject : object["m_Gameobjects"])
		{
			unsigned int GameObject_ID = jsonGameObject["GameObject_ID"];
			GameObject* pGameObject = ObjectManager::GetInstance()->CreateObject(GameObject_ID);
			pGameObject->SerializeIn(jsonGameObject);
		}
	}

	void Scene::SerializeOut(nlohmann::ordered_json& object)
	{
		// ���� : ȫ��ȯ, �� �������� ������Ʈ�� ��� ������Ʈ �Ŵ��� Vaild�� ����ǵ��� ���� �����Ͽ� �ش� �ҽ��� ����
		const std::unordered_map<unsigned int, GameObject*>& objectMaps = ObjectManager::GetInstance()->GetObjectMaps();
		object["SceneID"] = this->GetId();

		for (auto& pair : objectMaps)
		{
			nlohmann::ordered_json jsonGameObject;

			GameObject* gameObject = pair.second;
			gameObject->SerializeOut(jsonGameObject);
			object["m_Gameobjects"].push_back(jsonGameObject);
		}
	}
}
