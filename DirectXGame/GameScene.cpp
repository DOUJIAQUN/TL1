#include "GameScene.h"

using namespace KamataEngine;


void GameScene::ParseObjectRecursive(const json& object, LevelData* levelData) {
    assert(object.contains("type"));
    assert(object.contains("name"));
    assert(object.contains("transform"));

    ObjectData objData;
    objData.type = object["type"].get<std::string>();
    objData.name = object["name"].get<std::string>();

    const auto& t = object["transform"];
    objData.transform.translation = { t["translation"][0], t["translation"][1], t["translation"][2] };
    objData.transform.rotation = { t["rotation"][0], t["rotation"][1], t["rotation"][2] };
    objData.transform.scaling = { t["scaling"][0], t["scaling"][1], t["scaling"][2] };

    if (object.contains("file_name")) {
        objData.file_name = object["file_name"].get<std::string>();
    }

    levelData->objects.push_back(objData);

    // 子要素再帰処理
    if (object.contains("children")) {
        for (const json& child : object["children"]) {
            ParseObjectRecursive(child, levelData);
        }
    }

    // collider
    if (object.contains("collider")) {
        const json& collider = object["collider"];
        std::string type = collider["type"];
        Vector3 center = { collider["center"][0], collider["center"][1], collider["center"][2] };
        Vector3 size = { collider["size"][0], collider["size"][1], collider["size"][2] };
    }
}
GameScene::GameScene() {}

GameScene::~GameScene() {
     for (auto model : models_) {
        delete model;
    }
    models_.clear();
    worldTransforms_.clear();
}
void GameScene::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
    camera_.Initialize();
    // ==============================================
    // ステップ1: JSONファイルのデシリアライズ化
    // ==============================================
    const std::string fullpath = "Resources/levels/scene.json";
    std::ifstream file(fullpath);
    
    // ファイルオープン失敗チェック
    if (file.fail()) {
        assert(0); // 画像通りにハードアサーション
        return;
    }

    // 逆シリアライズ化
    json deserialized;
    file >> deserialized;
    file.close();

    // ==============================================
    // JSONデータ検証
    // ==============================================
    assert(deserialized.is_object()); // object型か
    assert(deserialized.contains("name")); // "name"があるか
    assert(deserialized["name"].is_string()); // 文字列か
    assert(deserialized["name"] == "scene"); // 値が"scene"か
    assert(deserialized.contains("objects")); // "objects"があるか
    assert(deserialized["objects"].is_array()); // 配列か

    // ==============================================
    // レベルデータ構造体への格納
    // ==============================================
    LevelData* levelData = new LevelData(); // 画像通りnewで生成
    levelData->name = deserialized["name"].get<std::string>();

    // オブジェクト配列を走査
    for (const json& object : deserialized["objects"]) {
       ParseObjectRecursive(object, levelData);
    }

    // ==============================================
    // ゲームオブジェクト生成
    // ==============================================
    for (const ObjectData& objData : levelData->objects) {
        if (objData.type == "MESH" && !objData.file_name.empty()) {
            Model* model = Model::CreateFromOBJ(objData.file_name);
            models_.emplace_back(model);

            auto wt = std::make_unique<WorldTransform>();
            wt->Initialize();
            wt->translation_ = objData.transform.translation;
            wt->rotation_ = objData.transform.rotation;
            wt->scale_ = objData.transform.scaling;
            wt->UpdateMatrix();
            worldTransforms_.emplace_back(std::move(wt));
        }
        else if (objData.type == "CAMERA") {
            // カメラ処理
            camera_.translation_ = objData.transform.translation;
            camera_.rotation_ = objData.transform.rotation;
        }
        else if (objData.type == "LIGHT") {
            // ライト処理
        }
    }


    // メモリ解放
    delete levelData;
}

void GameScene::Update() {
    camera_.UpdateMatrix();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw();
    for (size_t i = 0; i < models_.size(); ++i) {
        models_[i]->Draw(*worldTransforms_[i], camera_);
    }
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}