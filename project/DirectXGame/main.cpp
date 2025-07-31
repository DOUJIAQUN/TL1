#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"LE3C_17_トウ_カグン");
	GameScene* gameScene = nullptr;
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	gameScene = new GameScene();
	gameScene->Initialize();
	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		// ゲームシーンの毎フレーム処理
		gameScene->Update();
		// 描画開始
		dxCommon->PreDraw();
		// ゲームシーンの描画
		gameScene->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}
	delete gameScene;
	gameScene = nullptr;
	KamataEngine::Finalize();

	return 0;
}
