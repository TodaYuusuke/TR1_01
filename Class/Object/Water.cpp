#include "Water.h"
#include <Novice.h>
#include <cstdlib>

// コンストラクタ
Water::Water() {
	Initialize({ -10000.0f,-10000.0f });
	isDead = true;
}
// デストラクタ
Water::~Water() {

}

// 初期化
void Water::Initialize(Vector2 position) {

	isDead = false;

	// 生まれてからの経過フレーム
	elapsedFrame_ = 0;

	// 座標
	position_ = position;
	// 速度
	velocity_ = { 0.0f,0.0f };
	// 加速度
	//acceleration_ = { 0.0f, 9.8f / 60.0f };
	acceleration_ = { 0.0f,0.0f };

	mass = 1.0f;

	// 初期のサイズ
	size_ = 20;
	// 現在の透明度
	alpha_ = 0xAA;
}
// 更新
void Water::Update() {
	if (isDead) { return; }

	// 煙の動きをリアルにするための処理を追加していく

	//size_ += 0.05f;

	// 横の速度は徐々に0に戻っていく
	velocity_.x *= kHorizontalVelocityNegativeCoefficient;
	// 貫通しないように速度制限
	if (velocity_.x > 5.0f) { velocity_.x = 5.0f; }
	if (velocity_.x < -5.0f) { velocity_.x = -5.0f; }
	if (velocity_.y > 5.0f) { velocity_.y = 5.0f; }
	if (velocity_.y < -5.0f) { velocity_.y = -5.0f; }

	// 加速度を速度に追加
	velocity_.x += acceleration_.x;
	velocity_.y += acceleration_.y;
	// 速度を座標に追加
	position_.x += velocity_.x;
	position_.y += velocity_.y;


	// 画面外に出ないように
	//if (position_.x - (size_ / 2.0f) < 0) {
	//	position_.x = 0 + (size_ / 2.0f);
	//}
	//if (position_.x + (size_ / 2.0f) > 1280) {
	//	position_.x = 1280 - (size_ / 2.0f);
	//}
	//
	//if (position_.y - (size_ / 2.0f) < 0) {
	//	position_.y = 0 + (size_ / 2.0f);
	//}
	//if (position_.y + (size_ / 2.0f) > 720) {
	//	position_.y = 720 - (size_ / 2.0f);
	//}

	// 場外に出たらidDeadをtrueに
	if (position_.x + (size_ / 2.0f) < 0 || position_.x - (size_ / 2.0f) > 1280) {
		isDead = true;
	}
	if (position_.y + (size_ / 2.0f) < 0 || position_.y - (size_ / 2.0f) > 720) {
		isDead = true;
	}
}
// 描画
void Water::Draw(int texture) {
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

void Water::Noise(){
	float sway = (float)(rand() % 201 - 100);
	sway /= 1000.0f;
	velocity_.x += sway;
}



bool Water::shapeIsBox = false;
float Water::xNegativeCoefficient = 0.001f;
float Water::yNegativeCoefficient = 0.001f;
float Water::kHorizontalVelocityNegativeCoefficient = 0.85f;