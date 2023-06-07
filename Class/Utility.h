#pragma once
#include "Vector2.h"
#include <math.h>

class Utility
{
public: // メンバ関数

	// 任意のn個のデータから平均を求めて返す関数
    template<typename T>
    static T CalculateAverage(const T* data, int n)
    {
        T sum = data[0];
        for (int i = 1; i < n; ++i)
        {
            sum += data[i];
        }

        return sum / static_cast<T>(n);
    }

    // 当たり判定用構造体
    struct Collision
    {
        // 中心点
        Vector2 center;
        // サイズ（横幅、半分で半径）
        float size;
    };

    // 四角形同士の当たり判定を返す関数
    static bool CheckBoxCollision(Collision c1, Collision c2) {
        // 四角形の頂点座標を計算
        float c1Left = c1.center.x - c1.size;
        float c1Right = c1.center.x + c1.size;
        float c1Top = c1.center.y - c1.size;
        float c1Bottom = c1.center.y + c1.size;

        float c2Left = c2.center.x - c2.size;
        float c2Right = c2.center.x + c2.size;
        float c2Top = c2.center.y - c2.size;
        float c2Bottom = c2.center.y + c2.size;

        // 当たり判定の条件をチェック
        if (c1Left <= c2Right && c1Right >= c2Left && c1Top <= c2Bottom && c1Bottom >= c2Top) {
            // 四角形が重なっている場合
            return true;
        }
        else {
            // 四角形が重なっていない場合
            return false;
        }
    }

    // 円同士の当たり判定を返す関数
    static bool CheckEllipseCollision(Collision c1, Collision c2) {
        // 2つの円の中心点間の距離を計算
        float distance = (float)sqrt(pow(c2.center.x - c1.center.x, 2) + pow(c2.center.y - c1.center.y, 2));

        // 円の半径の合計を計算
        float radiusSum = c1.size / 2.0f + c2.size / 2.0f;

        // 当たり判定の条件をチェック
        if (distance <= radiusSum) {
            // 円が重なっている場合
            return true;
        }
        else {
            // 円が重なっていない場合
            return false;
        }
    }

    // 2点の間の逆ベクトルを返す関数
    static Vector2 CalculateReverseVector(Vector2& point1, Vector2& point2) {
        Vector2 reverseVector;
        reverseVector.x = point1.x - point2.x;
        reverseVector.y = point1.y - point2.y;
        return reverseVector;
    }
};

