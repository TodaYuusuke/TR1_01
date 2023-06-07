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
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();


	// 新しい煙を追加
	void AddEffect(Vector2 position);


private: // メンバ変数

	// 煙たち
	std::list<Smoke*> smoke_;

	// 煙のテクスチャ
	int smokeTexture;

private: // 静的なメンバ変数

	// 当たり判定の処理方法
	enum CollisionDetectionMode {
		FullDetection,		// 全検証
		OptimizedDetection,	// 4分木空間
		NoDetection			// 行わない
	};

	// 当たり判定の計測方法（trueならば全検証）
	bool isBruteForceDetection = false;

	// 4分木空間分割用のデバッグ表示切替 //

	// 分割線表示
	bool isShowDividingLine = false;
	// 線分4分木のモートン序列番号を表示
	bool isShowMortonNumber = false;

	// 4分木空間分割数
	// 空間数は4^nになる
	int kQuadTreeResolution = 3;

private: // 非公開の関数

	
	/// <summary>
	/// remove_ifに渡す関数ポインタ
	/// </summary>
	bool SmokeDelete(const Smoke* it);
	
	/// <summary>
	/// ImGui関連の処理をまとめた関数
	/// </summary>
	void CallImGui();

	
	/// <summary>
	/// 全当たり判定検証
	/// </summary>
	void BruteForceObjectCollisionDetection(std::list<Smoke*> smoke);

	/// <summary>
	/// 空間分割法をつかった当たり判定検証
	/// </summary>
	void OptimizedCollisionDetection();

	/// <summary>
	/// nを1bit飛びの32bit数値に変換する関数
	/// </summary>
	int BitSeparate32(int n);

	/// <summary>
	/// モートン序列を取得する関数
	/// </summary>
	int GetMortonOrder(int x, int y);
};
