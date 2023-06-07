#include "Smoke.h"
#include <Novice.h>
#include <cstdlib>

// コンストラクタ
Smoke::Smoke() {
	Initialize({ -10000.0f,-10000.0f });
	isDead = true;
}
// デストラクタ
Smoke::~Smoke() {

}

// 初期化
void Smoke::Initialize(Vector2 position) {

	isDead = false;

	// 生まれてからの経過フレーム
	elapsedFrame_ = 0;

	// 座標
	position_ = position;
	// 速度
	velocity_ = { 0.0f,0.0f };
	// 加速度
	acceleration_ = { 0.0f,-0.03f };

	// 初期のサイズ
	size_ = 10;
	// 現在の透明度
	alpha_ = 0xAA;
}
// 更新
void Smoke::Update() {
	if (isDead) { return; }

	// 煙の動きをリアルにするための処理を追加していく

	size_ += 0.01f;

	// 横の速度は徐々に0に戻っていく
	velocity_.x *= kHorizontalVelocityNegativeCoefficient;

	// 加速度を速度に追加
	velocity_.x += acceleration_.x;
	velocity_.y += acceleration_.y;
	// 速度を座標に追加
	position_.x += velocity_.x;
	position_.y += velocity_.y;


	// 場外に出たらidDeadをtrueに
	if (position_.x < 0 || position_.x > 1280) {
		isDead = true;
	}
	if (position_.y < 0 || position_.y > 720) {
		isDead = true;
	}
}
// 描画
void Smoke::Draw(int texture) {
	if (isDead) { return; }
	
	// 四角
	if (shapeIsBox) {
		Novice::DrawBox((int)(position_.x - size_ / 2.0f), (int)(position_.y - size_ / 2.0f), (int)size_, (int)size_, 0, 0xAAAAAA00 + alpha_, kFillModeSolid);
	}
	// 円
	else {
		//Novice::DrawEllipse((int)position_.x, (int)position_.y, (int)(size_ / 2.0f), (int)(size_ / 2.0f), 0.0f, 0xAAAAAA00 + alpha_, kFillModeSolid);
		Novice::DrawSprite((int)(position_.x - size_ / 2.0f), (int)(position_.y - size_ / 2.0f), texture, size_ / 256, size_ / 256, 0, 0xFFFFFF00 + alpha_);
	}
}



// - 非公開の関数 - //

void Smoke::Noise(){
	float sway = (float)(rand() % 201 - 100);
	sway /= 1000.0f;
	velocity_.x += sway;
}



bool Smoke::shapeIsBox = false;
float Smoke::kHitVelocityCoefficient = 0.001f;
float Smoke::kHorizontalVelocityNegativeCoefficient = 0.85f;