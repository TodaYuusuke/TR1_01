#include "Smoke.h"
#include <Novice.h>

// コンストラクタ
Smoke::Smoke() {
	Initialize({ -10000.0f,-10000.0f });
}
// デストラクタ
Smoke::~Smoke() {

}

// 初期化
void Smoke::Initialize(Vector2 position) {

	// 生まれてからの経過フレーム
	elapsedFrame_ = 0;

	// 座標
	position_ = position;
	// 速度
	velocity_ = { 0.0f,0.0f };
	// 加速度
	acceleration_ = { 0.0f,-1.0f };

	// 現在の透明度
	alpha_ = 0xFF;
}
// 更新
bool Smoke::Update() {

	// 加速度を速度に追加
	velocity_.x += acceleration_.x;
	velocity_.y += acceleration_.y;
	// 速度を座標に追加
	position_.x += velocity_.x;
	position_.y += velocity_.y;


	// 場外に出たらfalseを返す
	if (position_.x < 0 || position_.x > 1280) {
		return false;
	}
	if (position_.y < 0 || position_.y > 720) {
		return false;
	}

	return true;
}
// 描画
void Smoke::Draw() {
	Novice::DrawEllipse((int)position_.x, (int)position_.y, 5, 5, 0.0f, 0xAAAAAA00 + alpha_, kFillModeSolid);
}