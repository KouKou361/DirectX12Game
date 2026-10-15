#include "CharacterBase.h"
#include "Lib_Model.h"
#include "../Scene/SceneBase.h"
#include "../NavMesh.h"
#include "imgui.h"
CharacterBase::CharacterBase(SceneGame* sceneGame)
{
	this->sceneGame = sceneGame;


};
CharacterBase::~CharacterBase()
{
	if (modelData != nullptr)
	{
		delete modelData;
		modelData = nullptr;
	}

	if (anima != nullptr)
	{
		delete anima;
		anima = nullptr;
	}
}
//初期化
void CharacterBase::Initialize()
{
	CreateNavAgent();
}
//更新処理
void CharacterBase::Update()
{
	
	//無敵時間
	InvincibilityTimeUpdate();
	//平行移動
	ParallelMove();
	//回転する
	Turn(moveVec);
	//垂直移動
	VerticalMove();
	//行列計算
	UpdateMat();
}
//描画処理
void CharacterBase::Render()
{
	//Imgui
	DebugImgui();

	TK_Lib::Render::ModelRender(modelData,SpriteShaderType::PBR);

	TK_Lib::Debug3D::Circle(GetPos(), damageCollisionLength);
}

void CharacterBase::ResetMoveTarget()
{
	if (sceneGame->navMesh == nullptr)return;
	sceneGame->navMesh->GetCrowd()->resetMoveTarget(navAgentIndex);
}

void CharacterBase::SetQuaternion(const VECTOR4& quaternion)
{
	this->quaternion = quaternion;

	XMVECTOR Q = XMLoadFloat4(&this->quaternion);
	//クォオタニオンの値がバクっていないか確認
	assert(!XMQuaternionIsNaN(Q));


	XMMATRIX Qu = XMMatrixRotationQuaternion(Q);
	XMFLOAT4X4 m;
	XMStoreFloat4x4(&m, Qu);

	XMVECTOR Right = XMVectorSet(m._11, m._12, m._13, m._14);//右ベクトルのセット
	XMVECTOR Up = XMVectorSet(m._21, m._22, m._23, m._24);//上ベクトルのセット
	XMVECTOR Front = XMVectorSet(m._31, m._32, m._33, m._34);//前ベクトルのセット

	XMStoreFloat3(&right, Right);
	XMStoreFloat3(&up, Up);
	XMStoreFloat3(&front, Front);
}
//ナビエージェントの追加
void CharacterBase::CreateNavAgent()
{
	dtCrowdAgentParams ap;
	memset(&ap, 0, sizeof(ap));
	ap.radius = 0.5f;
	ap.height = 1.0f;
	ap.maxAcceleration = 99999999999999999998.0f;
	ap.maxSpeed = 10.5f;
	ap.collisionQueryRange = ap.radius * 12.0f;
	ap.pathOptimizationRange = ap.radius * 30.0f;
	ap.updateFlags = 0;
	ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
	ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
	ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
	ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
	ap.updateFlags |= DT_CROWD_SEPARATION;

	ap.obstacleAvoidanceType = 3;
	ap.separationWeight = 2.0f;

	if (sceneGame->navMesh != nullptr)
	{
		navAgentIndex = sceneGame->navMesh->AddAgent(GetPos().x, GetPos().y, GetPos().z, ap);
	}


}

void CharacterBase::DebugImgui()
{
	ImGui::Begin("config 1");

	if (ImGui::TreeNode(name.c_str())) {

		ImGui::Text("HP=%d", GetHP());
		ImGui::Text("POS=%f1,%f1,%f1", GetPos().x, GetPos().y, GetPos().z);
		ImGui::Text("MoveSpeed=%f", moveSpeed);
		ImGui::Text("invincibilityTime=%f", invincibilityTime);
		ImGui::SliderFloat("Model:AnimationRate", &modelData->GetAnimationSeconds(), 0.0f, modelData->GetAnimationSecondsLength());


		ImGui::TreePop();
	}


	ImGui::End();
}

//回転する
void CharacterBase::Turn(const VECTOR3& targetVec)
{
	VECTOR4 Q = GetQuaternion();
	XMVECTOR Qu = XMLoadFloat4(&Q);

	if (moveVec == VECTOR3{ 0,0,0 })return;
	XMVECTOR Vec;
	//進む方向をXZ平面に変換する
	{
		VECTOR3 vec = moveVec;
		vec.y = 0.0f;
		Vec = DirectX::XMLoadFloat3(&vec);
		XMVECTOR L_moveVec;
		L_moveVec = XMVector3Length(Vec);
		float L;
		XMStoreFloat(&L, L_moveVec);
		if (L <= 0.0f)
		{
			return;
		}
		Vec = XMVector3Normalize(Vec);
	}

	XMVECTOR front;
	//正面ベクトルを
	//XZ平面に変換する
	{
		VECTOR3 Front = GetFront();
		Front.y = 0.0f;
		front = DirectX::XMLoadFloat3(&Front);
		front = XMVector3Normalize(front);
	}
	//回転軸
	XMFLOAT3 Up = GetUp();
	XMVECTOR Axis = DirectX::XMLoadFloat3(&Up);


	//回転量
	XMVECTOR Dot = XMVector3Dot(front, Vec);
	float dot;
	XMStoreFloat(&dot, Dot);
	dot = acosf(dot);//ラジアンに変換

	//二つのベクトルが近似している時は直ぐにreturn
	if (isnan(fabsf(dot))) {
		return;
	}
	//二つのベクトルが近似している区切り
	constexpr float min = XMConvertToRadians(1);
	//二つのベクトルが近似している時は直ぐにreturn
	if ((fabsf(dot) < min)) {
		return;
	}

	XMVECTOR q;

	//回転軸の算出
	XMVECTOR Cross;
	{
		Cross = XMVector3Cross(front, Vec);
		Cross = XMVector3Normalize(Cross);
	}
	XMFLOAT3 cross;
	XMStoreFloat3(&cross, Cross);


	if (cross.y < 0)
	{
		q = XMQuaternionRotationAxis(-Axis, dot);
	}
	else
	{
		q = XMQuaternionRotationAxis(Axis, dot);
	}




	q = XMQuaternionMultiply(q, Qu);

	//補完させる
	q = XMQuaternionSlerp(Qu, q, turnSpeed);

	XMStoreFloat4(&Q, q);

	SetQuaternion(Q);

}

//行列計算
void CharacterBase::UpdateMat()
{
	XMMATRIX mat = XMMatrixIdentity();
	XMMATRIX scaleMat,rotationMat,tranceformMat;

	XMVECTOR quaternionMat;
	XMVECTOR Qu = XMLoadFloat4(&quaternion);
	//クォオタニオンの値がバクっていないか確認
	assert(!XMQuaternionIsNaN(Qu));
	scaleMat    = XMMatrixScaling(scale.x, scale.y, scale.z);
	rotationMat = XMMatrixRotationQuaternion(Qu);
	tranceformMat = XMMatrixTranslation(pos.x, pos.y, pos.z);
	mat = scaleMat * rotationMat * tranceformMat;

	XMFLOAT4X4 Mat; XMStoreFloat4x4(&Mat, mat);

	modelData->UpdateAnimation(TK_Lib::Window::GetElapsedTime());
	modelData->UpdateTransform(Mat);
}

//垂直移動
void CharacterBase::VerticalMove()
{
	return;

	XMFLOAT3 Start = { pos.x,pos.y + 10,pos.z };
	XMFLOAT3 End = { pos.x,pos.y - 10,pos.z };
	RayOut ray;
	//sceneGame->stageModel->RayPick(Start, End, ray);
	if (TK_Lib::Collision::ModelRayPick(Start, End, ray))
	{
		pos.y = ray.Pos.y;
	}

	//TK_Lib::Debug3D::Circle(VECTOR3{ ray.Pos.x,ray.Pos.y,ray.Pos.z }, 1.0f);
	TK_Lib::Debug3D::Line(VECTOR3{ Start.x,Start.y,Start.z }, VECTOR3{ End.x,End.y,End.z });



}
//無敵時間の
void CharacterBase::InvincibilityTimeUpdate()
{
	if (invincibilityTime >  0)
	{
		invincibilityTime -= TK_Lib::Window::GetElapsedTime();
	}

	if (invincibilityTime < 0)
	{
		invincibilityTime = 0;
	}
}

bool CharacterBase::OnDamage(const int& damage, const float& invincibilityTime)
{
	if (this->invincibilityTime > 0)return false;
	if (damage <= 0)return false;
	if (hp <= 0)return false;

	this->invincibilityTime = invincibilityTime;
	hp -= damage;

	return true;
}

