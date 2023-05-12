#pragma once
#include "Vector2.h"

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
	bool Update();
	// 描画
	void Draw();



private: // メンバ変数

	// 生まれてからの経過フレーム
	int elapsedFrame_;
	
	// 座標
	Vector2 position_;
	// 速度
	Vector2 velocity_;
	// 加速度
	Vector2 acceleration_;

	// 現在の透明度
	int alpha_;
};

