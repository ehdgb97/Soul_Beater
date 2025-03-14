#include "SceneLoader.h"

#include "ObjectManager.h"
#include "GameObject.h"
#include "RenderManger.h"
#include "Scene.h"
#include "FrameInformation.h"

#include <fstream>
#include <codecvt>

namespace d2dFramework
{

	void SceneLoader::LoadScene(Scene* outScene)
	{

		const unsigned int outSceneID = outScene->GetId();

		///���� �н� ã��!
		std::string mapFilePath;
		std::ifstream ifs(L"./json/MapList.json", std::ios::in);
		if (!ifs.is_open())
		{
			assert(false);
			throw std::runtime_error("Object not found."); // ������ �߻���Ŵ
		}
		std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		nlohmann::ordered_json MapListobj = nlohmann::ordered_json::parse(str);
		for (auto& JsonMapobj : MapListobj["MapList"])
		{
			unsigned int mapID = JsonMapobj["Map_ID"].get<unsigned int>();
			if (outSceneID == mapID)
			{
				mapFilePath = JsonMapobj["FilePath"].get<std::string>();
				break;
			}
		}
		if (!mapFilePath.empty())
		{
			std::ifstream ifs2(mapFilePath, std::ios::in);
			if (ifs2.is_open())
			{
				std::string mainstr((std::istreambuf_iterator<char>(ifs2)), std::istreambuf_iterator<char>());
				ifs2.close();
				nlohmann::ordered_json jsonGameObjectContainer = nlohmann::ordered_json::parse(mainstr);
				outScene->SerializeIn(jsonGameObjectContainer);
			}
		}

		///ã�� ����Path�� Json ���� 



	}


	void SceneLoader::SaveScene(Scene* outScene)
	{
		///�������� ����.
		unsigned int outSceneID = outScene->GetId();
		std::string mapFilePath;

		///�ʸ���Ʈjson���� ���ϸ� ��������.
		std::ifstream ifs(L"./json/MapList.json", std::ios::in);
		if (!ifs.is_open())
		{
			assert(false);
			throw std::runtime_error("Object not found."); // ������ �߻���Ŵ
		}
		std::string strJsonMapList((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		nlohmann::ordered_json jsonMaplist = nlohmann::ordered_json::parse(strJsonMapList);

		for (auto& jsonMapObj : jsonMaplist["MapList"])
		{
			unsigned int mapID = jsonMapObj["Map_ID"].get<unsigned int>();
			if (outSceneID == mapID)
			{
				mapFilePath = jsonMapObj["FilePath"].get<std::string>();

				break;
			}
		}
		if (!mapFilePath.empty())
		{
			nlohmann::ordered_json jsonGameObjectContainer;
			std::ofstream ofsmapFilePath(mapFilePath);
			outScene->SerializeOut(jsonGameObjectContainer);
			// ���ν� ��ü ���Ͽ� ����
			// ���� �ݱ�
			ofsmapFilePath << jsonGameObjectContainer.dump(4);
			ofsmapFilePath.close();
		}
		else
		{
			nlohmann::ordered_json jsonNewMapInfo;
			std::string tempA = "json/Map/Map";
			int tempB = outSceneID;
			std::string tempC = ".json";
			mapFilePath = tempA + std::to_string(tempB) + tempC;
			jsonNewMapInfo["Map_ID"] = outSceneID;
			jsonNewMapInfo["FilePath"] = mapFilePath;
			jsonMaplist["MapList"].push_back(jsonNewMapInfo);
			std::ofstream ofsMapList(L"json/MapList.json");
			ofsMapList << jsonMaplist.dump(4);
			ofsMapList.close();

			nlohmann::ordered_json jsonGameObjectContainer;
			std::ofstream ofsmapFilePath(mapFilePath);
			outScene->SerializeOut(jsonGameObjectContainer);
			ofsmapFilePath << jsonGameObjectContainer.dump(4);
			ofsmapFilePath.close();
		}
	}

	void SceneLoader::LoadAllAnimationAssets()
	{

		// �ִϸ��̼� ����Ʈ ��� ����Ʈ�� ���̽����·� ����.
		std::ifstream ifsAnimationAssetList(L"./json/AnimationAssetList.json", std::ios::in);
		if (!ifsAnimationAssetList.is_open())
		{
			assert(false);
			throw std::runtime_error("Object not found."); // ������ �߻���Ŵ
		}
		std::string strAnimationList((std::istreambuf_iterator<char>(ifsAnimationAssetList)), std::istreambuf_iterator<char>());
		ifsAnimationAssetList.close();
		nlohmann::ordered_json jsonAnimationAssetList = nlohmann::ordered_json::parse(strAnimationList);

		//����� ����Ʈ���̽��� ��ȸ�ϸ� ��� �ִϸ��̼� ������ Load �ϱ�
		for (auto jsonAnimationAssetFileInfo : jsonAnimationAssetList["AnimationAssetList"])
		{
			// AnimationAsset�� AnimationKey �� Path �� ����
			const std::string strAnimationKey = jsonAnimationAssetFileInfo["AnimationAssetKey"].get<std::string>();
			const std::string strAnimationAssetPath = jsonAnimationAssetFileInfo["FilePath"].get<std::string>();

			//Path�� ���� �ش� AnimationAsset��� json���·� ����
			std::ifstream ifsAnimationAsset(strAnimationAssetPath, std::ios::in);
			if (!ifsAnimationAsset.is_open())
				throw std::runtime_error("Object not found."); // ������ �߻���Ŵ
			std::string strAnimationAsset((std::istreambuf_iterator<char>(ifsAnimationAssetList)), std::istreambuf_iterator<char>());
			ifsAnimationAsset.close();
			nlohmann::ordered_json jsonAnimationAsset = nlohmann::ordered_json::parse(strAnimationAsset);

			// �ִϸ��̼� ���� �� FrameInformation ����(frameInformations vector)�� ���� vecFrameInformation.
			std::vector<std::vector<FrameInformation>> vecFrameInformation;
			for (auto jsonAnimationState : jsonAnimationAsset["AnimationState"])
			{
				//�ִϸ��̼� ���¿� ���� FrameInformation������ ���� frameInformations vector.
				std::vector<FrameInformation> frameInformations;
				for (auto jsonAnimationFrameInformation : jsonAnimationState["FrameInformations"])
				{
					FrameInformation frameInformation;
					frameInformation.AnimationTime = jsonAnimationFrameInformation["AnimationTime"];
					frameInformation.UVRectangle.left = jsonAnimationFrameInformation["UVRectangle_left"];
					frameInformation.UVRectangle.top = jsonAnimationFrameInformation["UVRectangle_top"];
					frameInformation.UVRectangle.right = jsonAnimationFrameInformation["UVRectangle_right"];
					frameInformation.UVRectangle.bottom = jsonAnimationFrameInformation["UVRectangle_left_bottom"];
					frameInformations.push_back(frameInformation);
				}

				vecFrameInformation.push_back(frameInformations);

			}

			// ����� �ִϸ��̼� Ű��, ���ϰ��, frameInformations�� ����Ͽ� Asset ����.
			const WCHAR* WCHARAnimationKey = reinterpret_cast<const WCHAR*>(strAnimationKey.c_str());
			const WCHAR* WCHARAnimePath = reinterpret_cast<const WCHAR*>(strAnimationAssetPath.c_str());
			RenderManager::GetInstance()->CreateAnimationAsset(WCHARAnimationKey, WCHARAnimePath, vecFrameInformation);
		}

	}

	void SceneLoader::LoadAllBitmaps()
	{
		// �ִϸ��̼� ����Ʈ ��� ����Ʈ�� ���̽����·� ����.
		std::ifstream ifsBitmapList(L"./json/BitmapList.json", std::ios::in);
		if (!ifsBitmapList.is_open())
		{
			assert(false);
			throw std::runtime_error("Object not found."); // ������ �߻���Ŵ
		}

		std::string strBitmapList((std::istreambuf_iterator<char>(ifsBitmapList)), std::istreambuf_iterator<char>());
		ifsBitmapList.close();
		nlohmann::ordered_json jsonBitmapList = nlohmann::ordered_json::parse(strBitmapList);

		//����� ����Ʈ���̽��� ��ȸ�ϸ� ��� �ִϸ��̼� ������ Load �ϱ�
		for (auto jsonBitmapInfomations : jsonBitmapList["BitmapList"])
		{
			// AnimationAsset�� BitmapKey �� BitmapPath �� ����
			std::string strBitmapKey = jsonBitmapInfomations["BitmapKey"].get<std::string>();
			std::string strBitmapPath = jsonBitmapInfomations["FilePath"].get<std::string>();
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			std::wstring wstrBitmapKey = converter.from_bytes(strBitmapKey);
			std::wstring wstrBitmapPath = converter.from_bytes(strBitmapPath);


			const WCHAR* WCHARAnimationKey = reinterpret_cast<const WCHAR*>(wstrBitmapKey.c_str());
			const WCHAR* WCHARAnimePath = reinterpret_cast<const WCHAR*>(wstrBitmapPath.c_str());
			bool a = !jsonBitmapInfomations.contains("FramesPath");
			if (!jsonBitmapInfomations.contains("FramesPath"))
			{
				HRESULT hr = RenderManager::GetInstance()->CreateD2DBitmapFromFile(WCHARAnimationKey, WCHARAnimePath);
				assert(SUCCEEDED(hr));
			}
			else
			{
				const std::string strFramesPath = jsonBitmapInfomations["FramesPath"].get<std::string>();
				std::ifstream ifsBitmapFramesPath(L"json/Bit.json", std::ios::in);
				if (!ifsBitmapFramesPath.is_open())
					throw std::runtime_error("Object not found."); // ������ �߻���Ŵ
				std::string strBitmapInfo((std::istreambuf_iterator<char>(ifsBitmapFramesPath)), std::istreambuf_iterator<char>());
				ifsBitmapFramesPath.close();
				nlohmann::ordered_json jsonBitmapFrameInfo = nlohmann::ordered_json::parse(strBitmapInfo);
				int a = 0;
				for (auto jsonBitmapFrame : jsonBitmapFrameInfo["Frames"])
				{
					D2D1_RECT_F rectSize;
					rectSize.left = jsonBitmapFrame["Rect_left"];
					rectSize.top = jsonBitmapFrame["Rect_top"];
					rectSize.right = jsonBitmapFrame["Rect_right"];
					rectSize.bottom = jsonBitmapFrame["Rect_bottom"];
					strBitmapKey = strBitmapKey + std::to_string(a);
					const WCHAR* newWCHARAnimationKey = reinterpret_cast<const WCHAR*>(strBitmapKey.c_str());

					///renderManager->CreateD2DBitmapFromFile(WCHARAnimationKey, WCHARAnimePath,rectSize);
					a++;



				}
			}
		}

	}
}
