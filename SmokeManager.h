#pragma once
#include "Smoke.h"
#include <list>

class SmokeManager
{
public: // メンバ関数

	// コンストラクタ
	SmokeManager();
	// デストラクタ
	~SmokeManager();

	// 初期化
	//void Initialize(Vector2 position_);
	// 更新
	void Update();
	// 描画
	void Draw();


	// 新しい煙を追加
	void MakeNewEffect(Vector2 position);


private: // メンバ変数

	// 煙たち
	std::list<Smoke*> smoke_;
};

