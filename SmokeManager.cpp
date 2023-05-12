#include "SmokeManager.h"
#include <Novice.h>

// コンストラクタ
SmokeManager::SmokeManager() {

}
// デストラクタ
SmokeManager::~SmokeManager() {
	for (Smoke* s : smoke_) {
		delete s;
	}
}



// 初期化
//void SmokeManager::Initialize(Vector2 position_) {
//	for (Smoke* s : smoke_) {
//		s->Initialize();
//	}
//}
// 更新
void SmokeManager::Update() {
	// 削除する量
	int deleteCount = 0;
	// 削除するポインタ
	std::list<Smoke**> deletePointer;

	for (Smoke* s : smoke_) {
		if (!s->Update()) {
			deleteCount++;
		}
	}

	Novice::ScreenPrintf(0, 0, "%d", (int)smoke_.size());
}
// 描画
void SmokeManager::Draw() {
	for (Smoke* s : smoke_) {
		s->Draw();
	}
}


// 新しい煙を追加
void SmokeManager::MakeNewEffect(Vector2 position) {
	// 煙を生成し、初期化
	Smoke* newSmoke = new Smoke();
	newSmoke->Initialize(position);

	// 登録する
	smoke_.push_back(newSmoke);
}