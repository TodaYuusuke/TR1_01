#include <Novice.h>
#include <imgui.h>
#include <cstdlib>
#include "Class/Object/WaterManager.h"
#include "Class/Timer.h"

const char kWindowTitle[] = "TR1_01_LE2B_21_トダ_ユウスケ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// 乱数初期化
	srand((unsigned int)time(nullptr));

	// 時間計測系 //
	Timer timer;
	// 処理回数
	long frame = 0;
	// 座標
	int x, y;

	// 煙
	WaterManager* waterManager = new WaterManager();

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();


		// 計測終了
		timer.End();
		// FPS系の情報描画
		timer.CallImGui();
		// 計測開始
		timer.Start();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// マウス座標取得
		Novice::GetMousePosition(&x, &y);

		// Rでリセット
		if (keys[DIK_R] && !preKeys[DIK_R]) {
			timer.Initialize();
			waterManager->Initialize();
		}
		// SPACEが押されている間、水（×5）追加
		if (keys[DIK_SPACE]) {
			for (int i = 0; i < 1; i++) {
				waterManager->AddEffect({ 1280.0f / 2.0f,720.0f / 2.0f });
			}
		}
		// RightClickが押されている間、カーソル位置に水（×5）追加
		if (Novice::IsPressMouse(1)) {
			for (int i = 0; i < 1; i++) {
				waterManager->AddEffect({ (float)x,(float)y });
			}
		}

		// カプセルを作成する
		if (Novice::IsTriggerMouse(2)) {
			waterManager->AddCapsule({ (float)x,(float)y });
		}
		else if (Novice::IsPressMouse(2)){
			waterManager->SetEndPositionCapsule({ (float)x,(float)y });
		}

		// 更新処理
		waterManager->Update();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// 背景描画
		Novice::DrawBox(0, 0, 1280, 720, 0, BLACK, kFillModeSolid);

		// 描画処理
		waterManager->Draw();

		// 処理回数+1
		frame++;

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}