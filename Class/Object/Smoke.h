#pragma once
#include "Vector2.h"
#include "Class/Utility.h"

class Smoke
{
public: // メンバ関数

	// コンストラクタ
	Smoke();
	// デストラクタ
	~Smoke();

	// 初期化
	void Initialize(Vector2 position);
	// 更新
	void Update();
	// 描画
	void Draw(int texture);

	bool GetisDead() const { return isDead; }
	Utility::Collision GetCollision() const { return { position_,size_ }; }
	void AddVelocity(Vector2 velocity) { 
		velocity_.x += velocity.x * kHitVelocityCoefficient;
		velocity_.y += velocity.y * kHitVelocityCoefficient;
	}

public: // Managerで書き換えてもらうSmoke共通の変数

	// コンフィグ系

	// 処理を行う形を選択するフラグ（trueならばBoxで、falseならばEllipseで処理を行う）
	static bool shapeIsBox;

	// ヒット時の粒子に付与されるベクトルの係数
	static float kHitVelocityCoefficient;
	// 横移動のマイナス係数
	static float kHorizontalVelocityNegativeCoefficient;

private: // メンバ定数

	// 1m ... 100ピクセル
	const static int kPixelPerMeter = 100;

private: // メンバ変数

	// 画面内に出た場合にtrue;
	bool isDead;

	// 生まれてからの経過フレーム
	int elapsedFrame_;
	
	// 座標
	Vector2 position_;
	// 速度
	Vector2 velocity_;
	// 加速度
	Vector2 acceleration_;

	// 現在のサイズ
	float size_;
	// 現在の透明度
	int alpha_;

private: // 非公開の関数

	// 動きにノイズをつける
	void Noise();
};

