#pragma once
#include "Water.h"
#include <list>
#include <vector>
#include <thread>
#include <map>

class WaterManager
{
public: // メンバ関数

	// コンストラクタ
	WaterManager();
	// デストラクタ
	~WaterManager();

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();


	// 新しい煙を追加
	void AddEffect(Vector2 position);

	// カプセルを追加
	void AddCapsule(Vector2 position);
	void SetEndPositionCapsule(Vector2 position);

private: // メンバ変数

	// 煙たち
	std::list<Water*> water_;

	struct MortonCapsule {
		int mortonNumber;
		Utility::CapsuleCollision collision;
	};

	// 地形（Capsule）
	std::list<MortonCapsule*> capsules_;

	// 並列処理用のスレッド
	//std::vector<std::thread> threads;

	// 煙のテクスチャ
	int waterTexture_
		;

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

	float t = 100.0f;

private: // 非公開の関数

	
	/// <summary>
	/// remove_ifに渡す関数ポインタ
	/// </summary>
	bool WaterDelete(const Water* it);
	
	/// <summary>
	/// ImGui関連の処理をまとめた関数
	/// </summary>
	void CallImGui();

	/// <summary>
	/// モートン序列を受け取る関数
	/// </summary>
	int GetMortonNumber(Vector2 leftUp, Vector2 rightDown);

	/// <summary>
	/// モートン序列をセットする関数
	/// </summary>
	void SetMortonNumber(std::map<int, std::list<Water*>>* cells, Water* water);
	
	/// <summary>
	/// 全当たり判定検証
	/// </summary>
	void BruteForceObjectCollisionDetection(std::list<Water*> smoke);

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

