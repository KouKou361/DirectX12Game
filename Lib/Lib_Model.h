#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "Lib_ModelResource.h"


class Graphics;
class Lib_ConstantBuffer;

class RayOut
{
public:
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 PolygonPos[3];//ポリゴンの位置
	float distance = 0.0f;//レイの視点から交点まで距離
	int materialIndex = -1;//衝突したポリゴンのマテリアル番号
};

// モデル
class Model
{
	
	
private:
	Graphics* graphics = nullptr;
public:
	//Model(Graphics* graphics,const char* filename);
	Model(Graphics* graphics, ModelResource* modelResource);
	~Model();
	friend ModelResource;

	struct Node
	{
		const char* name;
		Node* parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	local_transform;
		DirectX::XMFLOAT4X4	world_transform;

		std::vector<Node*>	children;

		int lowerBobyFlg = 0;//下半身かどうか
	};

	struct FrameResource
	{
		Lib_ConstantBuffer* constantBuffer=nullptr;
		vector<DirectX::XMFLOAT4X4> cbv_data;
	};

	struct Mesh
	{
		const ModelResource::Mesh* mesh;
		std::vector<FrameResource>				frame_resources;
	};

	struct AnimationSetting
	{
		int								m_current_animation = -1;//アニメーション番号
		float							m_current_seconds = 0.0f;//アニメーションにひつような時間
		bool							m_loop_animation = false;//ループするかどうか
		bool							m_end_animation = false;
	};

	// 行列計算
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// アニメーション
	bool IsPlayAnimation() const { return bobyAnimationSetting.m_current_animation >= 0; }

	void ChangeStatic(const DirectX::XMFLOAT4X4& transform);
	void SetStaticTranceform(const DirectX::XMFLOAT4X4& transform);
	void PlayAnimation(int animationIndex, bool loop = false, int lowerBobyAnimation =-1, bool lowerLoop = false, float blendSconds =0.01f);
	void StopAnimation();
	void UpdateAnimation(float elapsedTime);

	void SetOriginNode(const string &name);
	void SetLowerBody(const string& name);
	void SetLowerBody(Node* node);

	float &GetAnimationSeconds();
	float  GetAnimationSecondsLength();

	void UpperBobyUpdateAnimation(float elapsedTime, AnimationSetting& animation);
	void LowerBobyUpdateAnimation(float elapsedTime, AnimationSetting& animation);

	Model::Node* SearchNode(const string& searchName);

	// ノードリスト取得
	const std::vector<Node>& GetNodes() const { return m_nodes; }
	std::vector<Node>& GetNodes() { return m_nodes; }

	// メッシュリスト取得
	std::vector<Mesh>& GetMeshes() { return m_meshes; }

	// リソース取得
	const ModelResource* GetResource() const { return m_resource; }

	void  DeserializeModel() { m_resource->DeserializeModel(); };

	void RayPick(const XMFLOAT3& start, const XMFLOAT3& end, RayOut& ray);


private:
	void Create(Graphics* graphics);
	ModelResource*			m_resource;
	std::vector<Node>						m_nodes;
	std::vector<Mesh>						m_meshes;
	Graphics* saveGraphics;

	std::string originNodeName="";



	AnimationSetting bobyAnimationSetting;
	AnimationSetting lowerbobyAnimationSetting;



	bool							staticFlg = false;

	float animationBlendTime = 0.0f;
	float animationBlendSeconds = 0.0f;
};
