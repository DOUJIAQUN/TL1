#pragma once
#include"KamataEngine.h"

#include <fstream>
#include <cassert>
#include <nlohmann/json.hpp>
using namespace KamataEngine;
using json = nlohmann::json;
// トランスフォームデータ構造体（完全に画像通りに実装）
struct Transform {
    Vector3 translation;
    Vector3 rotation;
    Vector3 scaling;
};

// オブジェクトデータ構造体
struct ObjectData {
    std::string type;  // "type"
    std::string name;  // "name"
    Transform transform;
    std::string file_name; // "file_name"（オプション）
};

// レベルデータ構造体
struct LevelData {
    std::string name;  // "name"
    std::vector<ObjectData> objects; // "objects"配列
};
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	void ParseObjectRecursive(const json& object, LevelData* levelData);
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	std::vector<Model*> models_;
    std::vector<std::unique_ptr<WorldTransform>> worldTransforms_;
	Camera camera_;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};