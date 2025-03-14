#pragma once
#include "defalut.h"
#include "SpriteRenderer.h"
#include "eObjectType.h"

#include <d3d9.h>
#include <utility>

namespace d2dFramework
{
	class RenderManager;
	class SceneManager;
}


enum class IdSet
{
	DEFALUE = 0,
	PLAYERID,
	MONSTERID,
	ITEMID,
	END
};

// const
const int PLAYER_ID_START = 10000;
const int MONSTER_ID_START = 20000;
const int ITEM_ID_START = 30000;
const int TILE_ID_START = 40000;
const int OBJECT_COMPONENT_COUNT = 10;

class MapToolGui
{
public:
	const float WIDTH = 600.f;
	const float HEIGHT = 1000.f;

	static MapToolGui* mpInstance;

public:
	MapToolGui(d2dFramework::SceneManager* sceneManager);
	~MapToolGui();

public:
	void GuiRender();				// Gui ����
	void GuiMenu();					// Save & Load ( ���� ���� )
	void GridCreate();				// �׸��� ���� ( ���� �ʿ� )

	void ObjectTileSetting();		// ������Ʈ Ÿ�� ����
	void ObjectSetting();			// ������Ʈ ����
	void ObjectSpriteSetting();		// ������Ʈ ��������Ʈ ����

	void ComponentEdit();			// ������Ʈ ����
	void ColliderSetting();			// �ݶ��̴� ������Ʈ ����
	void SpriteSetting();			// ��������Ʈ ������Ʈ ����

	void ObjectImfomation();		// ������Ʈ ���� ���
	void TileObjectImfomation();	// Ÿ�� ������Ʈ ���� ���

	void ImGuiImageLoading();		// Gui �̹��� �ε�

public:
	bool GetIsCreateTileObject() { return mbIsCreateTileObjectButtonIstrue; }
	bool GetIsCreateTileCollider() { return mbIsCreateTileColliderIstrue; }
	bool GetIsDeleteTileObject() { return mbIsDeleteTileObjectButtonIstrue; }
	bool GetIsCreateObject() { return mbIsCreateObjectButtonIstrue; }
	bool GetIsDeleteObject() { return mbIsDeleteObjectButtonIstrue; }
	bool GetIsCreateCollider() { return mbIsColliderCreate; }
	bool GetIsCreateSprite() { return mbIsSpriteCreate; }
	bool GetIsImageLoading() { return mbIsImageLoading; }
	bool GetIsImageSeleted() { return mbIsImageSelected; }
	bool GetIsCreateScene() { return mbIsCreateScene; }
	bool GetIsSave() { return mbIsSaveButton; }
	bool GetIsLoad() { return mbIsLoadButton; }

public:
	// handle window creation & destruction
	void CreateHWindow(const char* windowName, const char* className);
	void DestroyHWindow();

	// handle device creation & destruction
	bool CreateDevice();
	void ResetDevice();
	void DestroyDevice();

	// handle ImGui creation & destruction
	void CreateImGui();
	void DestroyImGui();

	void BeginRender();
	void EndRender();
	void Render();

public:
	HWND GetHwnd() { return mHWnd; }
	WNDCLASSEXA GetWindowClass() { return mWindowClass; }
	POINTS GetGuiPosition() { return mPosition; }
	PDIRECT3D9 GetD3D() { return md3d; }
	LPDIRECT3DDEVICE9 GetDevice() { return mDevice; }
	D3DPRESENT_PARAMETERS GetPresentParameter() { return mPresentParameters; }

	void SetClickPoint(LPARAM clickpoint) { mPosition = MAKEPOINTS(clickpoint); }
	void SetBackBufferWidth(WORD lower, WORD high)
	{
		mPresentParameters.BackBufferWidth = lower;
		mPresentParameters.BackBufferWidth = high;
	}

private:
	// �߰� ȫ��ȯ
	d2dFramework::SceneManager* mSceneManager;

	// constant window size
	bool exit;

	bool bOnbutton;

	// winapi window vars
	HWND mHWnd;
	WNDCLASSEXA mWindowClass;

	// points for window movement
	POINTS mPosition;

	// direct x state vars
	PDIRECT3D9 md3d;
	LPDIRECT3DDEVICE9 mDevice;
	D3DPRESENT_PARAMETERS mPresentParameters;

public:
	// �� �� ID
	static int SceneId;
	static int SelectedSceneId;

	// create �� �� �����ǰ� �������� �ٷ� 2d �迭 ����
	static unsigned int mGridXCount;
	static unsigned int mGridYCount;
	static unsigned int mGridDistance;

	static std::vector<std::vector<bool>>		mbIsChecked;		// �̰� ���� ���鼭 Ŭ�� ó��
	static std::vector<std::vector<bool>>		mbIsObject;			// ����� �ش� ����ĭ�� ������Ʈ�� �����ϴ���
	static std::map<std::pair<int, int>, int>	mObjectIdMap;		// �ش� ĭ�� ������Ʈ�� �ִ���

	// Tile Bitmap ����
	static int BitmapU;
	static int BitmapV;
	static int BitmapXNumber;
	static int BitmapYNumber;

	// Tile ������Ʈ
	static std::vector<std::vector<bool>>		mbIsTileObject;		// ����� �ش� ����ĭ�� Ÿ�� ������Ʈ�� �����ϴ���
	static std::map<std::pair<int, int>, int>	mTileObjectIdMap;	// �ش� ĭ�� Ÿ�� ������Ʈ�� �ִ���
	static int				TileSpriteWidth;
	static int				TileSpriteHeight;
	static int				TileColliderWidth;
	static int				TileColliderHeight;
	static int				TileObjectItemCurrentIndex;

	// ������Ʈ�� Id��
	static IdSet mIdSetting;
	static int mPlayerId;
	static int mMonsterId;
	static int mItemId;
	static int mTileId;
	static d2dFramework::eObjectType ObjectType;		// ������Ʈ Ÿ��

	// ������Ʈ ������Ʈ ���� ����
	static bool bIsColliderCheck;
	static int ColliderWidth;
	static int ColliderHeight;

	// ��������Ʈ ������Ʈ ���� ����
	static bool bIsSpriteCheck;
	static d2dFramework::eSpriteType SpriteType;
	static int	CreateSpriteWidth;
	static int	CreateSpriteHeight;
	static int	SpriteWidth;
	static int	SpriteHeight;
	static int	ObjectItemCurrentIndex;

	// �׸��� ������Ʈ ���� ����
	static bool mbIsGridCheck;

	// ����
	static int ItemImfoCurrentIndex;

	// ImGui�� ����Ʈ �̸�(�Լ��� ȣȯ�� �� �ڷ��� ����Ʈ �̸�)
	static std::vector<std::string> ImageListName;
	static std::vector<std::wstring> WstringImageListName;
	static std::wstring wstrImageName;
	static std::wstring wstrImagePath;
	// ImGui �ؽ�Ʈ �ڽ��� �Էµ� static char
	static char ImageName[100];
	static char ImagePath[256];
	// �̹��� ����Ʈ ���� �� �̹��� Ŭ�� �� �̹��� ����
	static int ItemImageCurrentIndex;

private:
	// Ÿ�� ������Ʈ
	bool mbIsCreateTileObjectButtonIstrue;
	bool mbIsCreateTileColliderIstrue;
	bool mbIsDeleteTileObjectButtonIstrue;

	// ������Ʈ ���� �� ����
	bool mbIsCreateObjectButtonIstrue;
	bool mbIsDeleteObjectButtonIstrue;

	// ������Ʈ ����
	bool mbIsColliderCreate;
	bool mbIsSpriteCreate;

	// �̹��� �ε�
	bool mbIsImageLoading;
	bool mbIsImageSelected;

	// ���̺� & �ε� ��ư
	bool mbIsCreateScene;
	bool mbIsSaveButton;
	bool mbIsLoadButton;
};
