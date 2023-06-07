#include "Timer.h"
#include "Class/Utility.h"
#include "imgui.h"

Timer::Timer() {
    programStartTime_ = std::chrono::steady_clock::now();
}

void Timer::Initialize() {
    // プログラム実行開始時間
    programStartTime_ = std::chrono::steady_clock::now();
    // フレーム開始時間
    frameStartTime_ = {};
    // フレーム終了時間
    frameEndTime_ = {};

    // 次にデータを入れるindex
    frameDurationIndex_ = 0;
    // フレーム経過時間の配列
    for (int i = 0; i < kframeDurationSize_; i++) {
        frameDurations_[i] = 0;
    }

    // 経過フレーム
    elapsedFrame_ = 0;
    // 60フレームごとのFPS
    frameRate = 0;
}

void Timer::Start() {
    frameStartTime_ = std::chrono::steady_clock::now();
}

void Timer::End() {
    frameEndTime_ = std::chrono::steady_clock::now();
    std::chrono::duration<double> frameDuration = frameEndTime_ - frameStartTime_;
    frameDurations_[frameDurationIndex_++] = 1.0f / frameDuration.count();
    if (frameDurationIndex_ >= kframeDurationSize_) { frameDurationIndex_ = 0; }

    elapsedFrame_++;
    frameStartTime_ = frameEndTime_;
}

double Timer::GetFPS() {
    return Utility::CalculateAverage(frameDurations_, kframeDurationSize_);
}


double Timer::GetElapsedTimeMS() {
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsedTime = currentTime - programStartTime_;
    return elapsedTime.count();
}
double Timer::GetElapsedTimeS() {
    return GetElapsedTimeMS() / 1000.0;
}

double Timer::GetElapsedTimeM() {
    return GetElapsedTimeS() / 60.0;
}

double Timer::GetElapsedTimeH() {
    return GetElapsedTimeM() / 60.0;
}

void Timer::CallImGui() {
    // 60処理ごとに一度取得するFPS
    if (elapsedFrame_ % 60 == 0) {
        frameRate = GetFPS();
    }


    ImGui::Begin("Information");
    ImGui::Text("ElapsedTime ... %02d:%02d:%02d:%02d", (int)GetElapsedTimeH() % 24, (int)GetElapsedTimeM() % 60, (int)GetElapsedTimeS() % 60, (int)GetElapsedTimeMS() % 1000 / 10);
    ImGui::Text("FPS ... %.1f (%.1f)", frameRate, GetFPS());
    ImGui::Text("FrameTime ... %.1fms (%.1fms)", 1000.0f / frameRate, 1000.0f / GetFPS());
    ImGui::Text("ElapsedFrame ... %d", elapsedFrame_);
    ImGui::Text("");
    ImGui::Text("R ... Reset");
    ImGui::Text("Space ... AddEffect");
    ImGui::Text("RightClick ... AddEffect at Cursor");
    ImGui::Text("MiddleClick ... Create Capsule");
    ImGui::End();
}