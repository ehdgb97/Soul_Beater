#include "SoulBeaterProcessor.h"

#include "defalut.h"

#include "gui.h"

#include "MapToolWinApp.h"
#include"SceneLoader.h"

#include "ObjectManager.h"
#include "RenderManger.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "AABBCollider.h"
#include "CircleCollider.h"
#include "SpriteRenderer.h"
#include "Collider.h"
#include "GridComponent.h"

namespace mapTool
{
	SoulBeaterProcessor::SoulBeaterProcessor(UINT width, UINT height, std::wstring name)
		: GameProcessor(width, height, name)
		, mRect{}
		, mImGui(new MapToolGui(getSceneManager()))
	{
	}

	void SoulBeaterProcessor::Init(HWND hwnd)
	{
		const int canvasWidth = GetWidth() / 80, canvasHeight = GetHeight() / 80;

		using namespace d2dFramework;

		GameProcessor::Init(hwnd);
		getSceneManager()->CreateScene<Scene>(50000);
		// register Register Scene;

		mImGui->CreateHWindow("Map Tool Menu", "Map Tool Class");
		mImGui->CreateDevice();
		mImGui->CreateImGui();

		SceneLoader::LoadAllBitmaps();
	}

	void SoulBeaterProcessor::Update()
	{
		using namespace d2dFramework;

		// ���� ���� ���� ó���� �ش� �ε�Ʈ �ȿ��� ó���ϵ��� �ؾ��Ѵ�.
		RenderManager::GetInstance()->BitmapBeginDraw(); // ��� ������ �ʱ�ȭ
		{
			MouseClickCheck();
			GameProcessor::Update(); // ��� �����ӿ�ũ ������ �������� ������ ���ӷ��� 
			Render();
		}
		RenderManager::GetInstance()->BitmapEndDraw();
		RenderManager::GetInstance()->BeginDraw();
		RenderManager::GetInstance()->CopyBitmapRenderToHwndRender();
		RenderManager::GetInstance()->EndDraw();

		// switch the back buffer and the front buffer

		mImGui->BeginRender();
		mImGui->Render();
		ImGuiImageLoading();
		mImGui->EndRender();
	}

	void SoulBeaterProcessor::Release()
	{
		using namespace d2dFramework;

		GameProcessor::Release();

		mImGui->DestroyImGui();
		mImGui->DestroyDevice();
		mImGui->DestroyHWindow();
	}

	void SoulBeaterProcessor::Render()
	{
		POINT pos = d2dFramework::InputManager::GetInstance()->GetMousePos();
		d2dFramework::RenderManager::GetInstance()->DrawPoint((float)pos.x, (float)pos.y);

		// ������ �׸���
		{
			CreateScene();
			d2dFramework::RenderManager::GetInstance()->DrawGrid(0, 0, (float)MapToolGui::mGridXCount * MapToolGui::mGridDistance, (float)MapToolGui::mGridYCount * MapToolGui::mGridDistance, (float)MapToolGui::mGridDistance);
			CheckedRender();
			ColliderRender();
		}

		// Ÿ�� ������Ʈ ����
		{
			TileObjectCreate();
			TileObjectDelete();
			//TileColliderCreate();
		}

		// ������Ʈ ���� ( ������Ʈ ���� ) �� ����
		{
			ObjectCreate();
			ColliderSetting();
			SpriteSetting();
			ObjectDelete();
		}

		// ���̺�
		{
			SaveAndLoad();
		}

		d2dFramework::RenderManager::GetInstance()->SetStrokeWidth(2);
	}

	void SoulBeaterProcessor::MouseClickCheck()
	{
		using namespace d2dFramework;

		// ��ư Ŭ�� �� �׸��忡 �´� �� üũ
		if (InputManager::GetInstance()->GetKeyState(VK_LBUTTON) == eKeyState::Push && MapToolGui::mGridDistance != 0)
		{
			POINT point = InputManager::GetInstance()->GetMousePos();

			int mouseX = point.x;
			int mouseY = point.y;
			int arrX = mouseX / (float)MapToolGui::mGridDistance;
			int arrY = mouseY / (float)MapToolGui::mGridDistance;

			mRect.top = (float)arrY;
			mRect.left = (float)arrX;
			mRect.bottom = (float)arrY + MapToolGui::mGridDistance;
			mRect.right = (float)arrX + MapToolGui::mGridDistance;

			if (MapToolGui::mGridXCount > arrX && MapToolGui::mGridYCount > arrY)
			{
				if (MapToolGui::mbIsChecked[arrX][arrY] == true)
				{
					MapToolGui::mbIsChecked[arrX][arrY] = false;
					MapToolGui::ItemImfoCurrentIndex = 0;
				}
				else
					MapToolGui::mbIsChecked[arrX][arrY] = true;
			}
		}
	}

	void SoulBeaterProcessor::CheckedRender()
	{
		using namespace d2dFramework;

		for (int i = 0; i < MapToolGui::mbIsChecked.size(); i++)
		{
			for (int j = 0; j < MapToolGui::mbIsChecked[i].size(); j++)
			{
				if (MapToolGui::mbIsChecked[i][j] == true)
				{
					RenderManager::GetInstance()->SetStrokeWidth(5);
					D2D1_COLOR_F prevColor = d2dFramework::RenderManager::GetInstance()->SetColor({ 1,0,0,1 });
					RenderManager::GetInstance()->DrawRectangle((float)i * MapToolGui::mGridDistance, (float)j * MapToolGui::mGridDistance,
						(float)(i + 1) * MapToolGui::mGridDistance, (float)(j + 1) * MapToolGui::mGridDistance);
					d2dFramework::RenderManager::GetInstance()->SetColor(prevColor);
					RenderManager::GetInstance()->SetStrokeWidth(2);
				}
			}
		}
	}

	void SoulBeaterProcessor::ColliderRender()
	{
		using namespace d2dFramework;

		for (int i = 0; i < MapToolGui::mbIsChecked.size(); i++)
		{
			for (int j = 0; j < MapToolGui::mbIsChecked[i].size(); j++)
			{
				if (MapToolGui::mbIsObject[i][j] == true)
				{
					std::pair<int, int> key = std::make_pair(i, j);
					auto iter = MapToolGui::mObjectIdMap.find(key);

					GameObject* object = ObjectManager::GetInstance()->FindObjectOrNull(iter->second);
					CircleCollider* collider = object->GetComponent<CircleCollider>();

					if (collider != nullptr)
					{
						RenderManager::GetInstance()->DrawCircle(
							(float)i * MapToolGui::mGridDistance + MapToolGui::mGridDistance * 0.5f
							, (float)j * MapToolGui::mGridDistance + MapToolGui::mGridDistance * 0.5f
							, collider->GetRadius()
							, collider->GetRadius());
					}
				}
			}
		}
	}

	void SoulBeaterProcessor::TileObjectCreate()
	{
		using namespace d2dFramework;

		if (mImGui->GetIsCreateTileObject())
		{
			for (int i = 0; i < MapToolGui::mbIsChecked.size(); i++)
			{
				for (int j = 0; j < MapToolGui::mbIsChecked[i].size(); j++)
				{
					//üũ�� �Ǿ����鼭 ������Ʈ�� ���� ���� ������Ʈ ����
					if (MapToolGui::mbIsChecked[i][j] == true && MapToolGui::mbIsTileObject[i][j] == false)
					{
						//������Ʈ ����
						Vector2 pos =
						{
							i * static_cast<float>(MapToolGui::mGridDistance) - GetWidth() * 0.5f + MapToolGui::mGridDistance * 0.5f,
							j * -static_cast<float>(MapToolGui::mGridDistance) + GetHeight() * 0.5f - MapToolGui::mGridDistance * 0.5f
						};

						GameObject* Object = ObjectManager::GetInstance()->CreateObject(MapToolGui::mTileId);
						Object->SetObjectType(eObjectType::Background);
						MapToolGui::mTileObjectIdMap.insert(std::make_pair(std::make_pair(i, j), MapToolGui::mTileId++));
						MapToolGui::mbIsTileObject[i][j] = true;
						Transform* TransformComponent = Object->CreateComponent<Transform>(MapToolGui::mTileId++);
						GridComponent* Grid = Object->CreateComponent<GridComponent>(MapToolGui::mTileId++);
						Grid->SetCellDistance(MapToolGui::mGridDistance);
						Grid->SetMaxX(MapToolGui::mGridXCount);
						Grid->SetMaxY(MapToolGui::mGridYCount);
						Grid->SetPosition({ static_cast<unsigned int>(i), static_cast<unsigned int>(MapToolGui::mbIsTileObject[i].size() - j - 1) });
						TransformComponent->SetTranslate(pos);
						SpriteRenderer* ComponentRenderer = Object->CreateComponent<SpriteRenderer>(MapToolGui::mTileId++);
						ComponentRenderer->SetBitmap(RenderManager::GetInstance()->GetBitmapOrNull(MapToolGui::WstringImageListName[MapToolGui::TileObjectItemCurrentIndex].c_str()), MapToolGui::WstringImageListName[MapToolGui::TileObjectItemCurrentIndex]);
						ComponentRenderer->SetSize({ static_cast<float>(MapToolGui::TileSpriteWidth), static_cast<float>(MapToolGui::TileSpriteHeight) });
						ComponentRenderer->SetUVRectangle({ (float)((0 + MapToolGui::BitmapXNumber) * MapToolGui::BitmapU)
							,(float)((0 + MapToolGui::BitmapYNumber) * MapToolGui::BitmapV)
							,(float)((1 + MapToolGui::BitmapXNumber) * MapToolGui::BitmapU)
							,(float)((1 + MapToolGui::BitmapYNumber) * MapToolGui::BitmapV) });
						ComponentRenderer->SetSpriteType(eSpriteType::Sprite);
						ComponentRenderer->SetBaseColor({ 1,0,0,1 });

						if (mImGui->GetIsCreateTileCollider())
						{
							AABBCollider* collider = Object->CreateComponent<AABBCollider>(MapToolGui::mTileId++);
							collider->SetSize({ static_cast<float>(MapToolGui::mGridDistance), static_cast<float>(MapToolGui::mGridDistance) });
						}
					}
					// ������Ʈ�� ������ �Ǿ� �ִٸ� ��Ʈ�ʸ� ����
					else if (MapToolGui::mbIsChecked[i][j] == true && MapToolGui::mbIsTileObject[i][j] == true)
					{
						std::pair<int, int> key = std::make_pair(i, j);
						auto iter = MapToolGui::mTileObjectIdMap.find(key);

						GameObject* object = ObjectManager::GetInstance()->FindObjectOrNull(iter->second);

						if (object != nullptr)
						{
							SpriteRenderer* ComponentRenderer = object->GetComponent<SpriteRenderer>();
							ComponentRenderer->SetBitmap(RenderManager::GetInstance()->GetBitmapOrNull(MapToolGui::WstringImageListName[MapToolGui::TileObjectItemCurrentIndex].c_str()), MapToolGui::WstringImageListName[MapToolGui::TileObjectItemCurrentIndex]);
						}
					}
				}
			}
		}
	}

	void SoulBeaterProcessor::TileColliderCreate()
	{
		using namespace d2dFramework;

		if (mImGui->GetIsCreateTileCollider())
		{
			for (int i = 0; i < MapToolGui::mbIsChecked.size(); i++)
			{
				for (int j = 0; j < MapToolGui::mbIsChecked[i].size(); j++)
				{
					std::pair<int, int> key = std::make_pair(i, j);
					auto iter = MapToolGui::mTileObjectIdMap.find(key);

					// �ش� Ÿ���� üũ�ǰ� Ÿ�� ������Ʈ�� ���� ��
					if (MapToolGui::mbIsChecked[i][j] == true && MapToolGui::mbIsTileObject[i][j] == true && iter != MapToolGui::mTileObjectIdMap.end())
					{
						GameObject* object = ObjectManager::GetInstance()->FindObjectOrNull(iter->second);
						AABBCollider* Collider = object->GetComponent<AABBCollider>();
						int a = 0;
					}
				}
			}
		}
	}

	void SoulBeaterProcessor::TileObjectDelete()
	{
		// Ÿ�� ������Ʈ ����
		if (mImGui->GetIsDeleteTileObject())
		{
			for (int i = 0; i < MapToolGui::mbIsChecked.size(); i++)
			{
				for (int j = 0; j < MapToolGui::mbIsChecked[i].size(); j++)
				{
					std::pair<int, int> key = std::make_pair(i, j);
					auto iter = MapToolGui::mTileObjectIdMap.find(key);

					// üũ�Ǿ� �ְ� ������Ʈ�� �ְ� Id�� ���� ���� ������Ʈ ����
					if (MapToolGui::mbIsChecked[i][j] == true && MapToolGui::mbIsTileObject[i][j] == true && iter != MapToolGui::mTileObjectIdMap.end())
					{
						MapToolGui::mbIsTileObject[i][j] = false;

						d2dFramework::ObjectManager::GetInstance()->DeleteObject(iter->second);
						MapToolGui::mTileObjectIdMap.erase(iter);
					}
				}
			}
		}
	}

	void SoulBeaterProcessor::ObjectCreate()
	{
		using namespace d2dFramework;

		if (mImGui->GetIsCreateObject())
		{
			for (int i = 0; i < MapToolGui::mbIsChecked.size(); i++)
			{
				for (int j = 0; j < MapToolGui::mbIsChecked[i].size(); j++)
				{
					//üũ�� �Ǿ����鼭 ������Ʈ�� ���� ���� ������Ʈ ����
					if (MapToolGui::mbIsChecked[i][j] == true && MapToolGui::mbIsObject[i][j] == false)
					{
						//������Ʈ ����
						Vector2 pos =
						{
							i * static_cast<float>(MapToolGui::mGridDistance) - GetWidth() * 0.5f + MapToolGui::mGridDistance * 0.5f,
							j * -static_cast<float>(MapToolGui::mGridDistance) + GetHeight() * 0.5f - MapToolGui::mGridDistance * 0.5f
						};

						switch (MapToolGui::mIdSetting)		// ������Ʈ Ÿ�Ժ� Id, ������Ʈ Ÿ��, ������Ʈ ����
						{
						case IdSet::DEFALUE:
							break;
						case IdSet::PLAYERID:
						{
							GameObject* Object = ObjectManager::GetInstance()->CreateObject(MapToolGui::mPlayerId);
							Object->SetObjectType(eObjectType::Player);
							MapToolGui::mObjectIdMap.insert(std::make_pair(std::make_pair(i, j), MapToolGui::mPlayerId++));
							MapToolGui::mbIsObject[i][j] = true;

							Transform* TransformComponent = Object->CreateComponent<Transform>(MapToolGui::mPlayerId++);
							TransformComponent->SetTranslate(pos);
							SpriteRenderer* ComponentRenderer = Object->CreateComponent<SpriteRenderer>(MapToolGui::mPlayerId++);
							CircleCollider* collider = Object->CreateComponent<CircleCollider>(MapToolGui::mPlayerId++);
							GridComponent* Gird = Object->CreateComponent<GridComponent>(MapToolGui::mPlayerId++);

							if (MapToolGui::bIsSpriteCheck)		// ��������Ʈ ������Ʈ ����
							{
								ComponentRenderer->SetSize({ static_cast<float>(MapToolGui::CreateSpriteWidth), static_cast<float>(MapToolGui::CreateSpriteHeight) });
								ComponentRenderer->SetSpriteType(MapToolGui::SpriteType);
								ComponentRenderer->SetBitmap(RenderManager::GetInstance()->GetBitmapOrNull(MapToolGui::WstringImageListName[MapToolGui::ObjectItemCurrentIndex].c_str()), MapToolGui::WstringImageListName[MapToolGui::TileObjectItemCurrentIndex]);
								ComponentRenderer->SetUVRectangle({ 0,0,500,500 });
								ComponentRenderer->SetBaseColor({ 1,0,0,1 });
							}
							if (MapToolGui::bIsColliderCheck)	// �ݶ��̴� ������Ʈ ����
							{
								collider->SetRadius(MapToolGui::ColliderWidth);
								collider->SetRadius({ static_cast<float>(MapToolGui::ColliderWidth) });
							}
							if (MapToolGui::mbIsGridCheck)		// �׸��� ������Ʈ ����
							{
								Gird->SetCellDistance(MapToolGui::mGridDistance);
								Gird->SetMaxX(MapToolGui::mGridXCount);
								Gird->SetMaxY(MapToolGui::mGridYCount);
								Gird->SetPosition({ static_cast<unsigned int>(i), static_cast<unsigned int>(j) });
								TransformComponent->SetTranslate(pos);
							}
						}
						break;
						case IdSet::MONSTERID:
						{
							GameObject* Object = ObjectManager::GetInstance()->CreateObject(MapToolGui::mMonsterId);
							Object->SetObjectType(eObjectType::Enemy);
							MapToolGui::mObjectIdMap.insert(std::make_pair(std::make_pair(i, j), MapToolGui::mMonsterId++));
							MapToolGui::mbIsObject[i][j] = true;

							Transform* TransformComponent = Object->CreateComponent<Transform>(MapToolGui::mMonsterId++);
							TransformComponent->SetTranslate(pos);
							SpriteRenderer* ComponentRenderer = Object->CreateComponent<SpriteRenderer>(MapToolGui::mMonsterId++);
							CircleCollider* collider = Object->CreateComponent<CircleCollider>(MapToolGui::mMonsterId++);
							GridComponent* Gird = Object->CreateComponent<GridComponent>(MapToolGui::mMonsterId++);

							if (MapToolGui::bIsSpriteCheck)		// ��������Ʈ ������Ʈ ����
							{
								ComponentRenderer->SetSize({ static_cast<float>(MapToolGui::CreateSpriteWidth), static_cast<float>(MapToolGui::CreateSpriteHeight) });
								ComponentRenderer->SetSpriteType(MapToolGui::SpriteType);
								ComponentRenderer->SetBitmap(RenderManager::GetInstance()->GetBitmapOrNull(MapToolGui::WstringImageListName[MapToolGui::ObjectItemCurrentIndex].c_str()), MapToolGui::WstringImageListName[MapToolGui::TileObjectItemCurrentIndex]);
								ComponentRenderer->SetUVRectangle({ 0,0,500,500 });
								ComponentRenderer->SetBaseColor({ 1,0,0,1 });
							}
							if (MapToolGui::bIsColliderCheck)	// �ݶ��̴� ������Ʈ ����
							{
								collider->SetRadius({ static_cast<float>(MapToolGui::ColliderWidth) });
							}
							if (MapToolGui::mbIsGridCheck)		// �׸��� ������Ʈ ����
							{
								Gird->SetCellDistance(MapToolGui::mGridDistance);
								Gird->SetMaxX(MapToolGui::mGridXCount);
								Gird->SetMaxY(MapToolGui::mGridYCount);
								Gird->SetPosition({ static_cast<unsigned int>(i), static_cast<unsigned int>(j) });
								TransformComponent->SetTranslate(pos);
							}
						}
						break;
						case IdSet::ITEMID:
						{
							GameObject* Object = ObjectManager::GetInstance()->CreateObject(MapToolGui::mItemId);
							Object->SetObjectType(eObjectType::Item);
							MapToolGui::mObjectIdMap.insert(std::make_pair(std::make_pair(i, j), MapToolGui::mItemId++));
							MapToolGui::mbIsObject[i][j] = true;

							Transform* TransformComponent = Object->CreateComponent<Transform>(MapToolGui::mItemId++);
							TransformComponent->SetTranslate(pos);
							SpriteRenderer* ComponentRenderer = Object->CreateComponent<SpriteRenderer>(MapToolGui::mItemId++);
							CircleCollider* collider = Object->CreateComponent<CircleCollider>(MapToolGui::mItemId++);
							GridComponent* Gird = Object->CreateComponent<GridComponent>(MapToolGui::mItemId++);

							if (MapToolGui::bIsSpriteCheck)		// ��������Ʈ ������Ʈ ����
							{
								ComponentRenderer->SetSize({ static_cast<float>(MapToolGui::CreateSpriteWidth), static_cast<float>(MapToolGui::CreateSpriteHeight) });
								ComponentRenderer->SetSpriteType(MapToolGui::SpriteType);
								ComponentRenderer->SetBitmap(RenderManager::GetInstance()->GetBitmapOrNull(MapToolGui::WstringImageListName[MapToolGui::ObjectItemCurrentIndex].c_str()), MapToolGui::WstringImageListName[MapToolGui::TileObjectItemCurrentIndex]);
								ComponentRenderer->SetUVRectangle({ 0,0,500,500 });
								ComponentRenderer->SetBaseColor({ 1,0,0,1 });
							}
							if (MapToolGui::bIsColliderCheck)	// �ݶ��̴� ������Ʈ ����
							{
								collider->SetRadius(static_cast<float>(MapToolGui::ColliderWidth));
							}
							if (MapToolGui::mbIsGridCheck)		// �׸��� ������Ʈ ����
							{
								Gird->SetCellDistance(MapToolGui::mGridDistance);
								Gird->SetMaxX(MapToolGui::mGridXCount);
								Gird->SetMaxY(MapToolGui::mGridYCount);
								Gird->SetPosition({ static_cast<unsigned int>(i), static_cast<unsigned int>(j) });
								TransformComponent->SetTranslate(pos);
							}
						}
						break;
						}
					}
				}
			}
		}
	}

	void SoulBeaterProcessor::ObjectDelete()
	{
		// ������Ʈ ����
		if (mImGui->GetIsDeleteObject())
		{
			for (int i = 0; i < MapToolGui::mbIsChecked.size(); i++)
			{
				for (int j = 0; j < MapToolGui::mbIsChecked[i].size(); j++)
				{
					std::pair<int, int> key = std::make_pair(i, j);
					auto iter = MapToolGui::mObjectIdMap.find(key);

					// üũ�Ǿ� �ְ� ������Ʈ�� �ְ� Id�� ���� ���� ������Ʈ ����
					if (MapToolGui::mbIsChecked[i][j] == true && MapToolGui::mbIsObject[i][j] == true && iter != MapToolGui::mObjectIdMap.end())
					{
						MapToolGui::mbIsObject[i][j] = false;

						d2dFramework::ObjectManager::GetInstance()->DeleteObject(iter->second);
						MapToolGui::mObjectIdMap.erase(iter);
					}
				}
			}
		}
	}

	void SoulBeaterProcessor::ColliderSetting()
	{
		using namespace d2dFramework;

		if (mImGui->GetIsCreateCollider())
		{
			for (int i = 0; i < MapToolGui::mbIsChecked.size(); i++)
			{
				for (int j = 0; j < MapToolGui::mbIsChecked[i].size(); j++)
				{
					std::pair<int, int> key = std::make_pair(i, j);
					auto iter = MapToolGui::mObjectIdMap.find(key);

					// �ش� �迭�� üũ�Ǿ� �ְ� ������Ʈ�� �ְ� ������Ʈ ���̵� ���� �� ����
					if (MapToolGui::mbIsChecked[i][j] == true && MapToolGui::mbIsChecked[i][j] == true && iter != MapToolGui::mObjectIdMap.end())
					{
						GameObject* object = ObjectManager::GetInstance()->FindObjectOrNull(iter->second);
						CircleCollider* collider = object->GetComponent< CircleCollider>();

						// ������Ʈ�� �ݶ��̴��� ���� ������ ���� �ݶ��̴� ������ ���� �ݶ��̴� ���� ������ ����
						if (collider != nullptr)
						{
							collider->SetRadius({ static_cast<float>(MapToolGui::ColliderWidth) });
						}
					}
				}
			}
		}
	}

	void SoulBeaterProcessor::SpriteSetting()
	{
		using namespace d2dFramework;

		// �ش� ������Ʈ�� ��������Ʈ ������Ʈ ����
		if (mImGui->GetIsCreateSprite())
		{
			for (int i = 0; i < MapToolGui::mbIsChecked.size(); i++)
			{
				for (int j = 0; j < MapToolGui::mbIsChecked[i].size(); j++)
				{
					std::pair<int, int> key = std::make_pair(i, j);
					auto iter = MapToolGui::mObjectIdMap.find(key);

					// �ش� �迭�� üũ�Ǿ� �ְ� ������Ʈ�� �ְ� ������Ʈ ���̵� ���� �� ����
					if (MapToolGui::mbIsChecked[i][j] == true && MapToolGui::mbIsChecked[i][j] == true && iter != MapToolGui::mObjectIdMap.end())
					{
						GameObject* object = ObjectManager::GetInstance()->FindObjectOrNull(iter->second);
						SpriteRenderer* ComponentRenderer = object->GetComponent<SpriteRenderer>();

						// ������Ʈ�� ��������Ʈ�� ���� ������ ���� �ݶ��̴� ������ ���� ��������Ʈ ���� ������ ����
						if (ComponentRenderer != nullptr)
						{
							ComponentRenderer->SetSize({ static_cast<float>(MapToolGui::SpriteWidth), static_cast<float>(MapToolGui::SpriteHeight) });
							ComponentRenderer->SetSpriteType(MapToolGui::SpriteType);
							ComponentRenderer->SetBitmap(RenderManager::GetInstance()->GetBitmapOrNull(MapToolGui::WstringImageListName[MapToolGui::ObjectItemCurrentIndex].c_str()), MapToolGui::WstringImageListName[MapToolGui::TileObjectItemCurrentIndex]);
							ComponentRenderer->SetUVRectangle({ 0,0,500,500 });
							ComponentRenderer->SetBaseColor({ 1,0,0,1 });
						}
					}
				}
			}
		}
	}

	void SoulBeaterProcessor::ImGuiImageLoading()
	{
		using namespace d2dFramework;

		if (mImGui->GetIsImageLoading())
		{
			// ��Ʈ���� ã�� ��Ʈ���� ������ ����
			auto iter = RenderManager::GetInstance()->GetBitmapOrNull(MapToolGui::wstrImageName.c_str());

			if (iter == nullptr)
			{
				// ���� ��ο� �̹��� ���� ������ ����
				HRESULT hr = RenderManager::GetInstance()->CreateD2DBitmapFromFile(MapToolGui::wstrImageName.c_str(), MapToolGui::wstrImagePath.c_str());

				if (SUCCEEDED(hr))
				{
					// �̹��� ����Ʈ�� ���� ���ڿ� ����
					MapToolGui::ImageListName.push_back(MapToolGui::ImageName);
					MapToolGui::WstringImageListName.push_back(MapToolGui::wstrImageName);
				}
			}
		}

		if (mImGui->GetIsImageSeleted())
		{
			// ���� �� ������Ʈ ��Ʈ�� ����
			for (auto ObjectMap : MapToolGui::mObjectIdMap)
			{
				if (MapToolGui::mbIsChecked[ObjectMap.first.first][ObjectMap.first.second] == true)
				{
					GameObject* object = ObjectManager::GetInstance()->FindObjectOrNull(ObjectMap.second);

					if (object == nullptr)
						return;

					// ��Ʈ������ ����
					SpriteRenderer* ComponentRenderer = object->GetComponent<SpriteRenderer>();
					ComponentRenderer->SetSpriteType(eSpriteType::Sprite);
					ComponentRenderer->SetBitmap(RenderManager::GetInstance()->GetBitmapOrNull(MapToolGui::WstringImageListName[MapToolGui::ItemImageCurrentIndex].c_str()), MapToolGui::WstringImageListName[MapToolGui::TileObjectItemCurrentIndex]);
					ComponentRenderer->SetUVRectangle({ 0,0, 500, 500 });
				}
			}
		}
	}

	void SoulBeaterProcessor::CreateScene()
	{
		using namespace d2dFramework;

		if (mImGui->GetIsCreateScene())
		{
			// ���� ȫ��ȯ, ���ο� �׸��� ���� �� ���� id���� �ٲ�� ���� ����
			if (MapToolScene == nullptr)
			{
				MapToolScene = getSceneManager()->CreateScene<Scene>(MapToolGui::SceneId);
				getSceneManager()->SetCurrentScene(MapToolGui::SceneId);
			}
			else if (getSceneManager()->FindSceneOrNull(MapToolGui::SceneId) != nullptr)
			{
				MapToolScene->Exit();
				MapToolScene = getSceneManager()->FindSceneOrNull(MapToolGui::SceneId);
				getSceneManager()->SetCurrentScene(MapToolGui::SceneId);
			}
			else
			{
				MapToolScene->Exit();
				MapToolScene = getSceneManager()->CreateScene<Scene>(MapToolGui::SceneId);
				getSceneManager()->SetCurrentScene(MapToolGui::SceneId);
			}
		}
	}

	void SoulBeaterProcessor::SaveAndLoad()
	{
		using namespace d2dFramework;

		if (mImGui->GetIsSave())
		{
			if (MapToolScene != nullptr)
				SceneLoader::SaveScene(MapToolScene);
		}
		if (mImGui->GetIsLoad())
		{
			MapToolGui::SelectedSceneId = MapToolGui::SceneId;

			MapToolGui::mTileObjectIdMap.clear();

			//Scene* scene = getSceneManager()->GetCurrentScene();
			// ���� : ȫ��ȯ, id���� �Է��ؼ� load�� ��� ����
			if (MapToolScene == nullptr)
			{
				MapToolScene = getSceneManager()->CreateScene<Scene>(MapToolGui::SceneId);
				getSceneManager()->SetCurrentScene(MapToolGui::SceneId);
			}
			else if (MapToolScene->GetId() == MapToolGui::SceneId) // ���� ���� ���� ���̵�� �ε��� ��� ���� ��ȯ			
			{
				return;
			}
			else if (getSceneManager()->FindSceneOrNull(MapToolGui::SceneId) != nullptr)
			{
				MapToolScene->Exit();
				MapToolScene = getSceneManager()->FindSceneOrNull(MapToolGui::SceneId);
				getSceneManager()->SetCurrentScene(MapToolGui::SceneId);
			}
			else
			{
				MapToolScene->Exit();
				MapToolScene = getSceneManager()->CreateScene<Scene>(MapToolGui::SceneId);
				getSceneManager()->SetCurrentScene(MapToolGui::SceneId);
			}

			MapToolGui::mObjectIdMap.clear();
			MapToolGui::mTileObjectIdMap.clear();
			MapToolGui::mbIsChecked.clear();
			MapToolGui::mbIsObject.clear();
			MapToolGui::mbIsTileObject.clear();

			if (MapToolScene != nullptr)
			{
				ObjectManager::GetInstance()->Release();
				SceneLoader::LoadScene(MapToolScene);
			}

			// ���� ȫ��ȯ, ���� �����ǹǷ� ���� ������ ������Ʈ ť�� �޾Ƽ� �ε� ó��
			std::queue<GameObject*> createObjectQueue = ObjectManager::GetInstance()->GetCreateObjectQueue();

			// ���� ȫ��ȯ, ��� �ִ� ���� �ε����� ���� ������ ����� �׸��� ũ�⸦ �����ϵ��� ó��
			if (createObjectQueue.empty())
			{
				MapToolGui::mbIsChecked.resize(MapToolGui::mGridXCount, std::vector<bool>(MapToolGui::mGridYCount, false));
				MapToolGui::mbIsObject.resize(MapToolGui::mGridXCount, std::vector<bool>(MapToolGui::mGridYCount, false));
				MapToolGui::mbIsTileObject.resize(MapToolGui::mGridXCount, std::vector<bool>(MapToolGui::mGridYCount, false));

				return;
			}

			GameObject* object = createObjectQueue.front();
			GridComponent* grid = object->GetComponent<GridComponent>();

			MapToolGui::mbIsChecked.resize(grid->GetMaxX(), std::vector<bool>(grid->GetMaxY(), false));
			MapToolGui::mbIsObject.resize(grid->GetMaxX(), std::vector<bool>(grid->GetMaxY(), false));
			MapToolGui::mbIsTileObject.resize(grid->GetMaxX(), std::vector<bool>(grid->GetMaxY(), false));

			while (!createObjectQueue.empty())
			{
				GameObject* object = createObjectQueue.front();
				createObjectQueue.pop();

				unsigned int objectId = object->GetId();

				GridComponent* grid = object->GetComponent<GridComponent>();

				if (grid != nullptr)
				{
					MapToolGui::mGridDistance = grid->GetCellDistance();
					MapToolGui::mGridXCount = grid->GetMaxX();
					MapToolGui::mGridYCount = grid->GetMaxY();


					if (objectId >= TILE_ID_START)
					{
						MapToolGui::mTileObjectIdMap.insert(std::make_pair(std::make_pair(grid->GetPosition().X, grid->GetMaxY() - grid->GetPosition().Y - 1), objectId));
						MapToolGui::mbIsTileObject[grid->GetPosition().X][grid->GetMaxY() - grid->GetPosition().Y - 1] = true;
						MapToolGui::mTileId = objectId;
					}
					else if (objectId >= ITEM_ID_START)
					{
						MapToolGui::mObjectIdMap.insert(std::make_pair(std::make_pair(grid->GetPosition().X, grid->GetPosition().Y), objectId));
						MapToolGui::mbIsObject[grid->GetPosition().X][grid->GetPosition().Y] = true;
						MapToolGui::mPlayerId = objectId;
					}
					else if (objectId >= MONSTER_ID_START)
					{
						MapToolGui::mObjectIdMap.insert(std::make_pair(std::make_pair(grid->GetPosition().X, grid->GetPosition().Y), objectId));
						MapToolGui::mbIsObject[grid->GetPosition().X][grid->GetPosition().Y] = true;
						MapToolGui::mMonsterId = objectId;
					}
					else if (objectId >= PLAYER_ID_START)
					{
						MapToolGui::mObjectIdMap.insert(std::make_pair(std::make_pair(grid->GetPosition().X, grid->GetPosition().Y), objectId));
						MapToolGui::mbIsObject[grid->GetPosition().X][grid->GetPosition().Y] = true;
						MapToolGui::mPlayerId = objectId;
					}
				}
			}

			MapToolGui::mTileId += OBJECT_COMPONENT_COUNT;
			MapToolGui::mPlayerId += OBJECT_COMPONENT_COUNT;
			MapToolGui::mMonsterId += OBJECT_COMPONENT_COUNT;
			MapToolGui::mItemId += OBJECT_COMPONENT_COUNT;
		}
	}
}

