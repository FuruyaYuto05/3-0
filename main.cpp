#include <Novice.h>
#include <cstdint>
#include <imgui.h>  // ImGui利用

const char kWindowTitle[] = "学籍番号";

// ---------------- Vector3 ----------------
struct Vector3 {
	float x, y, z;
};

// ---------------- Matrix4x4 ----------------
struct Matrix4x4 {
	float m[4][4];
};

// 線形補間関数
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 result;
	result.x = v1.x + (v2.x - v1.x) * t;
	result.y = v1.y + (v2.y - v1.y) * t;
	result.z = v1.z + (v2.z - v1.z) * t;
	return result;
}

// 2次ベジェ曲線の描画関数
void DrawBezier(const Vector3& p0, const Vector3& p1, const Vector3& p2,
	const Matrix4x4& viewProjectionMatrix,
	const Matrix4x4& viewportMatrix,
	uint32_t color) {

	(void)viewProjectionMatrix;  // 未使用警告回避
	(void)viewportMatrix;

	const int kNumSamples = 100;
	for (int i = 0; i < kNumSamples; ++i) {
		float t0 = static_cast<float>(i) / kNumSamples;
		float t1 = static_cast<float>(i + 1) / kNumSamples;

		Vector3 a0 = Lerp(p0, p1, t0);
		Vector3 b0 = Lerp(p1, p2, t0);
		Vector3 bezier0 = Lerp(a0, b0, t0);

		Vector3 a1 = Lerp(p0, p1, t1);
		Vector3 b1 = Lerp(p1, p2, t1);
		Vector3 bezier1 = Lerp(a1, b1, t1);

		// -1〜1の座標空間を画面座標に変換
		int x0 = static_cast<int>((bezier0.x + 1.0f) * 640);
		int y0 = static_cast<int>((-bezier0.y + 1.0f) * 360);
		int x1 = static_cast<int>((bezier1.x + 1.0f) * 640);
		int y1 = static_cast<int>((-bezier1.y + 1.0f) * 360);

		Novice::DrawLine(x0, y0, x1, y1, color);
	}
}

// コントロールポイントに表示する黒い球（塗りつぶし円）
void DrawControlPointSphere(const Vector3& point) {
	int x = static_cast<int>((point.x + 1.0f) * 640);
	int y = static_cast<int>((-point.y + 1.0f) * 360);
	int radius = 8;

	Novice::DrawEllipse(x, y, radius, radius, 0.0f, 0x000000ff, kFillModeSolid);
	
}

// コントロールポイントの初期値
Vector3 controlPoints[3] = {
	{-0.8f, 0.58f, 1.0f},
	{1.76f, 1.0f, -0.3f},
	{0.94f, -0.7f, 2.3f},
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Matrix4x4 dummyViewProjection = {};
	Matrix4x4 dummyViewport = {};

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// ImGuiウィンドウでコントロールポイント調整
		ImGui::Begin("Control Points");
		ImGui::SliderFloat3("P0", &controlPoints[0].x, -3.0f, 3.0f);
		ImGui::SliderFloat3("P1", &controlPoints[1].x, -3.0f, 3.0f);
		ImGui::SliderFloat3("P2", &controlPoints[2].x, -3.0f, 3.0f);
		ImGui::End();

		// ベジェ曲線の描画
		DrawBezier(controlPoints[0], controlPoints[1], controlPoints[2], dummyViewProjection, dummyViewport, 0xFFFFFFFF);

		// コントロールポイントの黒い球描画
		for (int i = 0; i < 3; ++i) {
			DrawControlPointSphere(controlPoints[i]);
		}

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}
