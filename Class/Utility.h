#pragma once
#include "Vector2.h"
#define _USE_MATH_DEFINES
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
    struct CapsuleCollision
    {
        // 開始点
        Vector2 startPosision;
        // 終了点
        Vector2 endPosision;
        // 半径
        float radius;
    };


    // 四角形同士の当たり判定を返す関数k
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
    static bool CheckEllipseCollision(Collision c1, Collision c2, Vector2& correctionVec1, Vector2& correctionVec2) {
        //// 2つの円の中心点間の距離を計算
        //float distance = (float)sqrt(pow(c2.center.x - c1.center.x, 2) + pow(c2.center.y - c1.center.y, 2));

        //// 円の半径の合計を計算
        //float radiusSum = c1.size / 2.0f + c2.size / 2.0f;

        // 2つの円の中心点間のベクトルを計算
        Vector2 centerVec = { c2.center.x - c1.center.x, c2.center.y - c1.center.y };

        // 2つの円の中心点間の距離を計算
        float distance = Length(centerVec);

        // 円の半径の合計を計算
        float radiusSum = c1.size / 2.0f + c2.size / 2.0f;  

        // 当たり判定の条件をチェック
        if (distance <= radiusSum) {
            // 円が重なっている場合
            // 貫通を防止するベクトルを計算
            Vector2 penetrationVec = Normalize(centerVec);
            penetrationVec.x *= (radiusSum - distance);
            penetrationVec.y *= (radiusSum - distance);

            // 補正ベクトルを両方の円に返す
            correctionVec2 = penetrationVec;
            correctionVec1 = { -penetrationVec.x,-penetrationVec.y };  // 反対方向のベクトルをc2に返す

            return true;
        }
        else {
            // 円が重なっていない場合
            return false;
        }
    }

    // カプセルと円の当たり判定を検証する関数  
    static Vector2 CheckCapsuleCollision(Collision ellipse, CapsuleCollision capsule) {
        // 計算に必要なベクトルの変数
        Vector2 a, b, c, d, e, f, ba, cf;
        float t;
        a = capsule.startPosision;
        b = capsule.endPosision;
        c = ellipse.center;
        d.x = c.x - a.x;
        d.y = c.y - a.y;
        ba.x = b.x - a.x;
        ba.y = b.y - a.y;
        e = ba;

        // e を正規化
        e = Normalize(e);

        // tを求める
        t = Dot(d, e) / Length(ba);

        // tを補正
        Clamp(&t, 0, 1);

        // 線形補間
        f.x = (1.0f - t) * a.x + t * b.x;
        f.y = (1.0f - t) * a.y + t * b.y;

        // 距離を求める
        cf.x = c.x - f.x;
        cf.y = c.y - f.y;
        float distance = Length(cf);

        // 接触している場合 -> 円の色を変える
        if (distance < ellipse.size / 2.0f + capsule.radius) {
            // 補正ベクトルを計算して返す
            Vector2 normal = Normalize(cf);
            float penetration = (ellipse.size / 2.0f + capsule.radius) - distance;
            return { normal.x * penetration, normal.y * penetration };
        }
        else {
            // 衝突がない場合はゼロベクトルを返す
            return { 0.0f, 0.0f };
        }
    }

    // 2点の間の逆ベクトルを返す関数
    static Vector2 CalculateReverseVector(Vector2& point1, Vector2& point2) {
        Vector2 reverseVector;
        reverseVector.x = point1.x - point2.x;
        reverseVector.y = point1.y - point2.y;
        return reverseVector;
    }

    // Radian to Degree
    static float RadiantoDegree(float radian) {
        return radian * (180.0f / (float)M_PI);
    }
    // Degree to Radian
    static float DegreetoRadian(float degree) {
        return degree * ((float)M_PI / 180.0f);
    }

    // 角度（radian）を求める関数
    // from ... 座標１
    // to ... 座標２
    static float GetRadian(Vector2 from, Vector2 to) {
        // 角度を計算
        float radian = (float)atan2(to.y - from.y, to.x - from.x);

        // 180°未満の場合
        if (radian <= 0) {
            radian *= -1;
        }
        else {
            float def = (float)M_PI - radian;
            radian = def + (float)M_PI;
        }

        return radian;

    }
    // Vectorを回転させる関数
    static Vector2 TurnVector2(Vector2 pos, float radian) {
        float preX = pos.x;
        float preY = pos.y;

        pos.x = preX * cosf(radian) - preY * sinf(radian);
        pos.y = preY * cosf(radian) + preX * sinf(radian);

        return pos;
    }

    // 正規化したベクトルを求める関数
    static Vector2 Normalize(Vector2 vector) {

        float length = sqrtf(vector.x * vector.x + vector.y * vector.y);
        // 長さがゼロでない場合のみ、各成分を割る
        if (length != 0) {
            vector.x /= length;
            vector.y /= length;
        }

        return vector;
    }
    // 内積を求める関数
    static float Dot(Vector2 vector1, Vector2 vector2) {
        return vector1.x * vector2.x + vector1.y * vector2.y;
    }
    // 長さを求める関数
    static float Length(Vector2 vector) {
        return sqrtf((vector.x * vector.x) + (vector.y * vector.y));
    }
    // tの範囲を調節する関数
    static void Clamp(float* t, int min, int max) {
        // minより小さいならminに戻す
        if (*t < (float)min) {
            *t = (float)min;
        }
        // maxより大きいならmaxに戻す
        if (*t > (float)max) {
            *t = (float)max;
        }
    }
};