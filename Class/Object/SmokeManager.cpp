#include "SmokeManager.h"
#include <Novice.h>
#include <imgui.h>
#include "Class/Utility.h"
#include <map>

// コンストラクタ
SmokeManager::SmokeManager() {
	Initialize();
}
// デストラクタ
SmokeManager::~SmokeManager() {
	for (Smoke* s : smoke_) {
		delete s;
	}
}


// 初期化
void SmokeManager::Initialize() {
	for (Smoke* s : smoke_) {
		delete s;
	}

	smokeTexture = Novice::LoadTexture("./Resources/Smoke.png");
}

// 更新
void SmokeManager::Update() {

	// ImGuiを呼び出す
	CallImGui();

	// 当たり判定を検証
	if (isBruteForceDetection) {
		BruteForceObjectCollisionDetection(smoke_);
	}
	else {
		OptimizedCollisionDetection();
	}

	// 更新処理
	for (Smoke* s : smoke_) {
		s->Update();
	}

	// 削除処理
	smoke_.remove_if([this](Smoke* smoke) {
		return SmokeManager::SmokeDelete(smoke);
	});
}

void SmokeManager::Draw() {
	// 4分木分割法の場合、分割数に応じて基準用の線を描画
	if (isShowDividingLine && !isBruteForceDetection) {
		// 描画用の線の数
		int lineNum = 0;
		for (int i = 0; i < kQuadTreeResolution; i++) {
			lineNum += (int)powf(2.0f, (float)i);
		}

		// 線の幅
		float minLength = 1280.0f / (lineNum + 1.0f);

		for (int i = 1; i <= lineNum; i++) {
			Novice::DrawLine((int)minLength * i, 0, (int)minLength * i, 720, WHITE);
			Novice::DrawLine(0, (int)minLength * i, 1280, (int)minLength * i, WHITE);
		}
	}

	for (Smoke* s : smoke_) {
		s->Draw(smokeTexture);
	}
}

void SmokeManager::AddEffect(Vector2 position) {
	// 煙を生成し、初期化
	Smoke* newSmoke = new Smoke();

	// 指定された座標から左右にノイズをつける
	Vector2 makePos = position;
	makePos.x += rand() % 21 - 10;
	makePos.y += rand() % 21 - 10;
	newSmoke->Initialize(makePos);

	// 登録する
	smoke_.push_back(newSmoke);
}



bool SmokeManager::SmokeDelete(const Smoke* it) {
	return it->GetisDead();
}

void SmokeManager::CallImGui() {
	// パラメータ操作用ImGUI
	ImGui::Begin("ParameterChange");
	ImGui::Text("SmokeCount ... %d", smoke_.size());
	ImGui::Text("");
	// 形状切り替え
	ImGui::Checkbox("Shape is Box?", &Smoke::shapeIsBox);
	// 当たり判定計算式切り替え
	ImGui::Checkbox("CollisionDetection is BruteForce?", &isBruteForceDetection);
	if (!isBruteForceDetection) {
		// デバッグ用の情報表示切り替え
		ImGui::Checkbox("Show DividingLine?", &isShowDividingLine);
		ImGui::Checkbox("Show MortonNumber?", &isShowMortonNumber);
		ImGui::Text("kQuadTreeResolution");
		ImGui::SliderInt(" ", &kQuadTreeResolution, 1, 8);
	}
	ImGui::Text("");
	ImGui::Text("khitVelocityCoefficient");
	ImGui::SliderFloat("  ", &Smoke::kHitVelocityCoefficient, 0.0f, 0.01f);
	ImGui::Text("kHorizontalVelocityNegativeCoefficient");
	ImGui::SliderFloat("   ", &Smoke::kHorizontalVelocityNegativeCoefficient, 0.0f, 0.85f);
	ImGui::Text("");
	// 
	ImGui::End();
}


void SmokeManager::BruteForceObjectCollisionDetection(std::list<Smoke*> smoke) {
	for (auto it1 = smoke.begin(); it1 != smoke.end(); ++it1) {
		for (auto it2 = std::next(it1); it2 != smoke.end(); ++it2) {
			// 2つのSmokeインスタンスの当たり判定を取得
			Utility::Collision collision1 = (*it1)->GetCollision();
			Utility::Collision collision2 = (*it2)->GetCollision();

			if (Smoke::shapeIsBox) {
				// 四角形同士の当たり判定を行う
				if (Utility::CheckBoxCollision(collision1, collision2)) {
					// お互いに逆ベクトルをVelocityに追加
					(*it1)->AddVelocity(Utility::CalculateReverseVector(collision1.center, collision2.center));
					(*it2)->AddVelocity(Utility::CalculateReverseVector(collision2.center, collision1.center));
				}
			}
			else {
				// 円同士の当たり判定を行う
				if (Utility::CheckEllipseCollision(collision1, collision2)) {
					// お互いに逆ベクトルをVelocityに追加
					(*it1)->AddVelocity(Utility::CalculateReverseVector(collision1.center, collision2.center));
					(*it2)->AddVelocity(Utility::CalculateReverseVector(collision2.center, collision1.center));
				}
			}
		}
	}
}

void SmokeManager::OptimizedCollisionDetection() {
	
	// オブジェクトを分割した空間に配置する
	std::map<int, std::list<Smoke*>> cells{};

	for (Smoke* s : smoke_) {
		// 空間１つの辺の長さ（画面を覆う最小の正方形（1280*1280）で計算する）
		float cellSize = 1280.0f / powf(2.0f, (float)kQuadTreeResolution);

		// 左上の座標
		Vector2 leftUp = s->GetCollision().center;
		leftUp.x -= s->GetCollision().size / 2.0f;
		leftUp.y -= s->GetCollision().size / 2.0f;
		// 右下の座標
		Vector2 rightDown = s->GetCollision().center;
		rightDown.x += s->GetCollision().size / 2.0f;
		rightDown.y += s->GetCollision().size / 2.0f;

		// モートン空間番号を出すために座標を変換
		leftUp.x /= cellSize;
		leftUp.y /= cellSize;
		rightDown.x /= cellSize;
		rightDown.y /= cellSize;

		// モートン空間番号を計算
		int leftUpMortonNum = GetMortonOrder((int)leftUp.x, (int)leftUp.y);
		int rightDownMortonNum = GetMortonOrder((int)rightDown.x, (int)rightDown.y);

		// 排他的論理和を求める
		int xorMortonNum = leftUpMortonNum ^ rightDownMortonNum;
		// 空間レベル
		int spaceLevel = kQuadTreeResolution;

		// 空間レベルを出す
		while (xorMortonNum > 0) {
			xorMortonNum >>= 2; 
			spaceLevel--;
		}

		// 領域の所属するモートン空間番号を出す
		int mortonNum = rightDownMortonNum >> ((kQuadTreeResolution - spaceLevel) * 2);
		// 線形4分木に直す
		mortonNum += (int)((powf(4.0f, (float)(spaceLevel)) - 1.0f) / 3.0f);
		// デバッグ用にモートン序列番号を表示
		if (isShowMortonNumber) {
			Novice::ScreenPrintf((int)s->GetCollision().center.x - 15, (int)s->GetCollision().center.y - 10, "%d", mortonNum);
		}
		cells[mortonNum].push_back(s);
	}


	// - ヒット判定 - //

	// 空間の最大分割数
	int maxResolution = 0;
	for (int i = 1; i <= kQuadTreeResolution; i++) {
		maxResolution += (int)powf(4.0f, (float)i);
	}

	// 検索するモートン空間番号
	int mortonNum = 0;
	// 上位空間のオブジェクトリスト
	std::list<Smoke*> ObjList{};

	while (true)
	{
		// 現在の空間内に登録されているオブジェクト同士を衝突
		if (!cells[mortonNum].empty()) {

			BruteForceObjectCollisionDetection(cells[mortonNum]);
			// スタック（リスト）に登録されているオブジェクトとの衝突判定
			if (!ObjList.empty()) {
				for (Smoke* currentSpaceSmoke : cells[mortonNum]) {
					for (Smoke* obj : ObjList) {
						// 2つのSmokeインスタンスの当たり判定を取得
						Utility::Collision collision1 = currentSpaceSmoke->GetCollision();
						Utility::Collision collision2 = obj->GetCollision();

						if (Smoke::shapeIsBox) {
							// 四角形同士の当たり判定を行う
							if (Utility::CheckBoxCollision(collision1, collision2)) {
								// お互いに逆ベクトルをVelocityに追加
								currentSpaceSmoke->AddVelocity(Utility::CalculateReverseVector(collision1.center, collision2.center));
								obj->AddVelocity(Utility::CalculateReverseVector(collision2.center, collision1.center));
							}
						}
						else {
							// 円同士の当たり判定を行う
							if (Utility::CheckEllipseCollision(collision1, collision2)) {
								// お互いに逆ベクトルをVelocityに追加
								currentSpaceSmoke->AddVelocity(Utility::CalculateReverseVector(collision1.center, collision2.center));
								obj->AddVelocity(Utility::CalculateReverseVector(collision2.center, collision1.center));
							}
						}
					}
				}
			}

		}

		// 次の小空間が4分木分割数を超えていなければ移動
		if ((mortonNum << 2) + 1 < maxResolution) {
			// スタックリストに現在の空間レベル内のSmokeを追加
			for (Smoke* currentSpaceSmoke : cells[mortonNum]) {
				ObjList.push_back(currentSpaceSmoke);
			}
			mortonNum = (mortonNum << 2) + 1;
			// 最初に戻る
			continue;
		}
		// そうでない場合は次のモートン番号に移動する
		else if(mortonNum % 4 != 0) {
			mortonNum++;
			// 最初に戻る
			continue;
		}

		// 上の空間に所属する小空間をすべて検証し終わった場合

		// １つ上の空間に戻る
		do {
			mortonNum = (mortonNum - 1) >> 2;
			// スタックからオブジェクトを外す
			size_t PopNum = cells[mortonNum].size();
			for (int i = 0; i < PopNum; i++) {
				ObjList.pop_back();
			}
			// 戻った空間がその空間の最後の数値の場合 -> もう一度戻る
		} while (mortonNum % 4 == 0);

		// 次のモートン番号へ進む
		mortonNum++;

		// 戻った空間がルート空間（-1）だった場合 -> ループ終了
		if (mortonNum == 0) {
			break;
		}
	}
}

int SmokeManager::BitSeparate32(int n)
{
	n = (n | (n << 8)) & 0x00ff00ff;
	n = (n | (n << 4)) & 0x0f0f0f0f;
	n = (n | (n << 2)) & 0x33333333;
	return (n | (n << 1)) & 0x55555555;
}

int SmokeManager::GetMortonOrder(int x, int y) {
	return BitSeparate32(x) | (BitSeparate32(y) << 1);
}