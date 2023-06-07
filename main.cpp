#include <Novice.h>
#include <imgui.h>
#include <cstdlib>
#include "Class/Object/SmokeManager.h"
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


	// 煙
	SmokeManager* smokeManager = new SmokeManager();

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();


		// 計測終了
		timer.End();
		// FPS系の情報描画
		timer.Show();
		// 計測開始
		timer.Start();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// F5でリセット
		if (keys[DIK_R] && !preKeys[DIK_R]) {
			timer.Initialize();
			smokeManager->Initialize();
		}
		// SPACEが押されている間、煙（×5）追加
		if (keys[DIK_SPACE]) {
			for (int i = 0; i < 5; i++) {
				smokeManager->AddEffect({ 1280.0f / 2.0f,720.0f });
			}
		}
		// RightClickが押されている間、カーソル位置に煙（×5）追加
		if (Novice::IsPressMouse(1)) {
			int x, y;
			Novice::GetMousePosition(&x, &y);
			for (int i = 0; i < 1; i++) {
				smokeManager->AddEffect({ (float)x,(float)y });
			}
		}


		// 更新処理
		smokeManager->Update();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// 描画処理
		smokeManager->Draw();

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