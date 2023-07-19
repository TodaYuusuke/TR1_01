#include "WaterManager.h"
#include <Novice.h>
#include <imgui.h>
#include "Class/Utility.h"
#include <cmath>

// コンストラクタ
WaterManager::WaterManager() {
	Initialize();
}
// デストラクタ
WaterManager::~WaterManager() {
	for (Water* s : water_) {
		delete s;
	}
	for (MortonCapsule* c : capsules_) {
		delete c;
	}
}


// 初期化
void WaterManager::Initialize() {
	for (Water* s : water_) {
		delete s;
	}
	water_.clear();
	for (MortonCapsule* c : capsules_) {
		delete c;
	}
	capsules_.clear();
	
	waterTexture_ = Novice::LoadTexture("./Resources/Water.png");


	// 外枠用のカプセルをセット
	AddCapsule({ 20.0f,360.0f });
	SetEndPositionCapsule({ 20.0f,700.0f });
	AddCapsule({ 1260.0f,360.0f });
	SetEndPositionCapsule({ 1260.0f,700.0f });
	AddCapsule({ 20.0f,700.0f });
	SetEndPositionCapsule({ 1260.0f,700.0f });
}

// 更新
void WaterManager::Update() {

	// ImGuiを呼び出す
	CallImGui();

	// 当たり判定を検証
	if (isBruteForceDetection) {
		BruteForceObjectCollisionDetection(water_);
	}
	else {
		OptimizedCollisionDetection();
	}

	// 更新処理
	for (Water* s : water_) {
		s->Update();
	}

	// 削除処理
	water_.remove_if([this](Water* smoke) {
		return WaterManager::WaterDelete(smoke);
	});
}

void WaterManager::Draw() {
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

	for (MortonCapsule* c : capsules_) {
		// 始点から終点までの角度
		float angle = -Utility::GetRadian(c->collision.startPosision, c->collision.endPosision);

		// 円の上下の点を求める
		Vector2 capsuleLineUpPos = { 0,c->collision.radius };
		Vector2 capsuleLineDownPos = { 0,-c->collision.radius };

		// 始点から終点までの角度分回転させる
		capsuleLineUpPos = Utility::TurnVector2(capsuleLineUpPos, angle);
		capsuleLineDownPos = Utility::TurnVector2(capsuleLineDownPos, angle);

		// カプセルと線の描画
		Novice::DrawEllipse((int)c->collision.startPosision.x, (int)c->collision.startPosision.y, (int)c->collision.radius, (int)c->collision.radius, 0, WHITE, kFillModeWireFrame);
		Novice::DrawEllipse((int)c->collision.endPosision.x, (int)c->collision.endPosision.y, (int)c->collision.radius, (int)c->collision.radius, 0, WHITE, kFillModeWireFrame);
		Novice::DrawLine((int)(capsuleLineUpPos.x + c->collision.startPosision.x), (int)(capsuleLineUpPos.y + c->collision.startPosision.y), (int)(capsuleLineUpPos.x + c->collision.endPosision.x), (int)(capsuleLineUpPos.y + c->collision.endPosision.y), WHITE);
		Novice::DrawLine((int)(capsuleLineDownPos.x + c->collision.startPosision.x), (int)(capsuleLineDownPos.y + c->collision.startPosision.y), (int)(capsuleLineDownPos.x + c->collision.endPosision.x), (int)(capsuleLineDownPos.y + c->collision.endPosision.y), WHITE);

		// デバッグ用にモートン序列番号を表示
		if (isShowMortonNumber) {
			//// カプセルのx座標の範囲を求める
			//float minX = min(capsules.back().collision.startPosision.x, capsules.back().collision.endPosision.x);
			//float maxX = max(capsules.back().collision.startPosision.x, capsules.back().collision.endPosision.x);

			//// カプセルのy座標の範囲を求める
			//float minY = min(capsules.back().collision.startPosision.y, capsules.back().collision.endPosision.y);
			//float maxY = max(capsules.back().collision.startPosision.y, capsules.back().collision.endPosision.y);

			//// 最小の矩形の左上座標と右下座標を設定
			//Vector2 leftUp = { minX - *capsules.back().collision.radius, minY - capsules.back().collision.radius };
			//Vector2 rightBottom = { maxX + capsules.back().collision.radius, maxY + capsules.back().collision.radius };

			//Novice::ScreenPrintf((int)leftUp.x - 10, (int)leftUp.y - 10, "%d", GetMortonNumber(leftUp, rightBottom));
		}	
	}

	for (Water* s : water_) {
		s->Draw(waterTexture_);
	}
}

void WaterManager::AddEffect(Vector2 position) {
	// 煙を生成し、初期化
	Water* newWater = new Water();

	// 指定された座標から左右にノイズをつける
	Vector2 makePos = position;
	makePos.x += rand() % 21 - 10;
	makePos.y += rand() % 21 - 10;
	newWater->Initialize(makePos);

	// 登録する
	water_.push_back(newWater);
}

void WaterManager::AddCapsule(Vector2 position) {
	// カプセルを生成し、初期化
	MortonCapsule* newCapsule = new MortonCapsule();
	newCapsule->mortonNumber = 0;
	newCapsule->collision = { position, position, 20 };
	capsules_.push_back(newCapsule);
}
void WaterManager::SetEndPositionCapsule(Vector2 position) {
	capsules_.back()->collision.endPosision = position;

	// カプセルのx座標の範囲を求める
	float minX = min(capsules_.back()->collision.startPosision.x, capsules_.back()->collision.endPosision.x);
	float maxX = max(capsules_.back()->collision.startPosision.x, capsules_.back()->collision.endPosision.x);

	// カプセルのy座標の範囲を求める
	float minY = min(capsules_.back()->collision.startPosision.y, capsules_.back()->collision.endPosision.y);
	float maxY = max(capsules_.back()->collision.startPosision.y, capsules_.back()->collision.endPosision.y);

	// 最小の矩形の左上座標と右下座標を設定
	Vector2 leftUp = { minX - capsules_.back()->collision.radius, minY - capsules_.back()->collision.radius };
	Vector2 rightBottom = { maxX + capsules_.back()->collision.radius, maxY + capsules_.back()->collision.radius };

	capsules_.back()->mortonNumber = GetMortonNumber(leftUp, rightBottom);
}



bool WaterManager::WaterDelete(const Water* it) {
	return it->GetisDead();
}

void WaterManager::CallImGui() {
	// パラメータ操作用ImGUI
	ImGui::Begin("ParameterChange");
	ImGui::Text("WaterCount ... %d", water_.size());
	ImGui::Text("");
	ImGui::DragFloat("t", &t, 1.0f);
	ImGui::SliderFloat("x", &Water::xNegativeCoefficient, 0.0f, 0.01f);
	// 形状切り替え
	ImGui::Checkbox("Shape is Box?", &Water::shapeIsBox);
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
	ImGui::Text("NegativeCoefficient");
	ImGui::SliderFloat("x", &Water::xNegativeCoefficient, 0.0f, 0.01f);
	ImGui::SliderFloat("y", &Water::yNegativeCoefficient, 0.0f, 0.01f);
	ImGui::Text("kHorizontalVelocityNegativeCoefficient");
	ImGui::SliderFloat("  ", &Water::kHorizontalVelocityNegativeCoefficient, 0.0f, 0.85f);
	ImGui::Text("");
	// 
	ImGui::End();
}


void WaterManager::BruteForceObjectCollisionDetection(std::list<Water*> water) {
	for (auto it1 = water.begin(); it1 != water.end(); ++it1) {
		Vector2 sumA = { 0.0f,0.0f };

		for (auto it2 = std::next(it1); it2 != water.end(); ++it2) {
			// 2つのWaterインスタンスの当たり判定を取得
			Utility::Collision collision1 = (*it1)->GetCollision();
			Utility::Collision collision2 = (*it2)->GetCollision();

			if (Water::shapeIsBox) {
				// 四角形同士の当たり判定を行う
				if (Utility::CheckBoxCollision(collision1, collision2)) {
					// お互いに逆ベクトルをVelocityに追加
					(*it1)->AddVelocity(Utility::CalculateReverseVector(collision1.center, collision2.center));
					(*it2)->AddVelocity(Utility::CalculateReverseVector(collision2.center, collision1.center));
				}
			}
			else {
				Vector2 correctionVec1, correctionVec2;

				// 円同士の当たり判定を行う
				if (Utility::CheckEllipseCollision(collision1, collision2, correctionVec1, correctionVec2)) {
					//(*it1)->AddVelocity(correctionVec1);
					//(*it2)->AddVelocity(correctionVec2);

					// 粒子間の距離と方向ベクトルを計算
					float dx = (*it2)->position_.x - (*it1)->position_.x;
					float dy = (*it2)->position_.y - (*it1)->position_.y;
					float distance = std::sqrtf(dx * dx + dy * dy);

					// ナビエストークス方程式に基づいて加速度を計算
					float r2 = distance * distance;
					float invMass = 1.0f / (*it2)->mass;

					// 粘度係数
					float viscosity = 1.0f;

					float pressure = 2.0f * viscosity * invMass * ((*it1)->velocity_.x * dx + (*it1)->velocity_.y * dy) / r2;
					float repulsion = t * invMass / r2;

					sumA.x -= pressure * dx + repulsion * dx;
					sumA.y -= pressure * dy + repulsion * dy;
				}

				// カプセルとの当たり判定を検証する
				for (MortonCapsule* capsule : capsules_) {
					// Smokeインスタンスの当たり判定を取得
					Utility::Collision collision = (*it1)->GetCollision();

					// 四角の時は実装してないので、円のときのみ行う
					if (!Water::shapeIsBox) {
						// 円とカプセルの当たり判定を行う
						(*it1)->HitCapsule(Utility::CheckCapsuleCollision(collision, capsule->collision));
					}
				}
			}
		}

		if (sumA.x == 0.0f && sumA.y == 0.0f) {
			sumA.y = 9.8f;
		}

		sumA.x /= 60.0f;
		sumA.y /= 60.0f;
		(*it1)->acceleration_ = sumA;
	}
}

int WaterManager::GetMortonNumber(Vector2 leftUp, Vector2 rightDown) {
	// 空間１つの辺の長さ（画面を覆う最小の正方形（1280*1280）で計算する）
	float cellSize = 1280.0f / powf(2.0f, (float)kQuadTreeResolution);

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

	return mortonNum;
}

void WaterManager::SetMortonNumber(std::map<int, std::list<Water*>>* cells, Water* water) {
	// 左上の座標
	Vector2 leftUp = water->GetCollision().center;
	leftUp.x -= water->GetCollision().size / 2.0f;
	leftUp.y -= water->GetCollision().size / 2.0f;
	// 右下の座標
	Vector2 rightDown = water->GetCollision().center;
	rightDown.x += water->GetCollision().size / 2.0f;
	rightDown.y += water->GetCollision().size / 2.0f;

	// 領域の所属するモートン空間番号を出す
	int mortonNum = GetMortonNumber(leftUp, rightDown);

	// デバッグ用にモートン序列番号を表示
	if (isShowMortonNumber) {
		Novice::ScreenPrintf((int)water->GetCollision().center.x - 5, (int)water->GetCollision().center.y - 5, "%d", mortonNum);
	}
	// 登録
	(*cells)[mortonNum].push_back(water);
}

void WaterManager::OptimizedCollisionDetection() {

	// オブジェクトを分割した空間に配置する
	std::map<int, std::list<Water*>> cells{};
	//// スレッド
	//std::vector<std::thread> threads;
	//// スレッドごとに分割して処理
	//const int numThreads = std::thread::hardware_concurrency(); // 使用可能なスレッド数を取得

	for (Water* water : water_) {
		SetMortonNumber(&cells, water);
	}

	// スレッドの終了待機
	//for (std::thread& thread : threads) {
	//	thread.join();
	//}

	// - ヒット判定 - //

	// 空間の最大分割数
	int maxResolution = 0;
	for (int i = 1; i <= kQuadTreeResolution; i++) {
		maxResolution += (int)powf(4.0f, (float)i);
	}

	// 検索するモートン空間番号
	int mortonNum = 0;
	// 上位空間のオブジェクトリスト
	std::list<Water*> ObjList{};


	while (true)
	{
		// 現在の空間内に登録されているオブジェクト同士を衝突
		if (!cells[mortonNum].empty()) {
			BruteForceObjectCollisionDetection(cells[mortonNum]);
			for (Water* currentSpaceSmoke : cells[mortonNum]) {
				// スタック（リスト）に登録されているオブジェクトとの衝突判定
				if (!ObjList.empty()) {

					for (Water* obj : ObjList) {
						// 2つのSmokeインスタンスの当たり判定を取得
						Utility::Collision collision1 = currentSpaceSmoke->GetCollision();
						Utility::Collision collision2 = obj->GetCollision();

						if (Water::shapeIsBox) {
							// 四角形同士の当たり判定を行う
							if (Utility::CheckBoxCollision(collision1, collision2)) {
								// お互いに逆ベクトルをVelocityに追加
								currentSpaceSmoke->AddVelocity(Utility::CalculateReverseVector(collision1.center, collision2.center));
								obj->AddVelocity(Utility::CalculateReverseVector(collision2.center, collision1.center));
							}
						}
						else {
							Vector2 correctionVec1, correctionVec2;

							// 円同士の当たり判定を行う
							if (Utility::CheckEllipseCollision(collision1, collision2, correctionVec1, correctionVec2)) {
								currentSpaceSmoke->AddVelocity(correctionVec1);
								obj->AddVelocity(correctionVec2);
							}
						}
					}
				}

				// カプセルとの当たり判定を検証する
				for (MortonCapsule* capsule : capsules_) {
					// Smokeインスタンスの当たり判定を取得
					Utility::Collision collision = currentSpaceSmoke->GetCollision();

					// 四角の時は実装してないので、円のときのみ行う
					if (!Water::shapeIsBox) {
						// 円とカプセルの当たり判定を行う
						currentSpaceSmoke->HitCapsule(Utility::CheckCapsuleCollision(collision, capsule->collision));
					}
				}
			}
		}

		// 次の小空間が4分木分割数を超えていなければ移動
		if ((mortonNum << 2) + 1 < maxResolution) {
			// スタックリストに現在の空間レベル内のSmokeを追加
			for (Water* currentSpaceSmoke : cells[mortonNum]) {
				ObjList.push_back(currentSpaceSmoke);
			}
			mortonNum = (mortonNum << 2) + 1;
			// 最初に戻る
			continue;
		}
		// そうでない場合は次のモートン番号に移動する
		else if (mortonNum % 4 != 0) {
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

int WaterManager::BitSeparate32(int n) {
	n = (n | (n << 8)) & 0x00ff00ff;
	n = (n | (n << 4)) & 0x0f0f0f0f;
	n = (n | (n << 2)) & 0x33333333;
	return (n | (n << 1)) & 0x55555555;
}

int WaterManager::GetMortonOrder(int x, int y) {
	return BitSeparate32(x) | (BitSeparate32(y) << 1);
}