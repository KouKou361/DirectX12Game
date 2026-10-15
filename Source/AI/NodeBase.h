#pragma once
#include <string>
#include <vector>
#include <memory>
#include "BehaviorTree.h"
#include "./Action/ActionBase.h"
#include "./Judge/JudgementBase.h"

using namespace std;


class NodeBase
{
public:
	std::vector<shared_ptr<NodeBase>>   children;// 子ノード
protected:

	string                   name="";	//ノードの名前
	BehaviorTree::SelectRule selectRule= BehaviorTree::SelectRule::NONE;//ノードの選択ルール
	NodeBase* parent = nullptr;//親ノード

	unique_ptr<ActionBase>   action = nullptr;//実行処理
	unique_ptr<JudgementBase> judgement = nullptr;//判定処理

	NodeBase* sibling = nullptr;		// 兄弟ノード

	unsigned int			 priority=0;		// 優先順位
	int						 hierarchyNo=0;	// 階層番号
	bool                     oldSelect = true;     //過去に選択されたかどうか
public:
	NodeBase() {};
	 //コンストラクタ
	NodeBase(std::string name, NodeBase* parent, NodeBase* sibling, const int priority,
		BehaviorTree::SelectRule selectRule, JudgementBase* judgment, ActionBase* action, const  int hierarchyNo, const bool OnOff = true) :
		name(name), parent(parent), sibling(sibling), priority(priority),
		selectRule(selectRule), hierarchyNo(hierarchyNo), 
		children(NULL), oldSelect(OnOff)
	{
		this->judgement.reset(judgment);
		this->action.reset(action);
	}
	// デストラクタ
	~NodeBase() {};
	// 名前ゲッター
	inline std::string GetName() { return name; }
	// 親ノードゲッター
	inline NodeBase* GetParent() { return parent; }
	// 子ノードゲッター
	inline NodeBase* GetChild(const int index) { return children.at(index).get(); };
	// 兄弟ノードゲッター
	inline NodeBase* GetSibling() { return sibling; }
	// ノード検索
	NodeBase* SearchNode(std::string searchName);
	// 階層番号ゲッター
	inline int GetHirerchyNo() { return hierarchyNo; }
	// 優先順位ゲッター
	inline int GetPriority() { return priority; }
	// 子ノード追加
	void AddChild(shared_ptr<NodeBase> child) { children.push_back(child); }
		//ノード推論
	NodeBase* Inference(BehaviorData* data);
	//ノードのアクション
	ActionBase::State Run(EnemyBase* enemy);

	//アクション開始処理
	void NodeActionStart() { action->Start(); }
	//アクション終了処理
	void NodeActionEnd() { action->End(); }
	//Debug用ImGui
	void ImguiDebug() { if (action)action->DebugImgui(); };
	//Debug用Render
	void RenderDebug() { if (action)action->DebugImgui(); };


private:
	//ランダムに一つ選択する
	NodeBase* SelectRandom(std::vector<NodeBase*> *list);
	//優先順位で選択する
	NodeBase* SelectPriority(std::vector<NodeBase*> *list);
	//シーケンス
	NodeBase* SelectSequence(std::vector<NodeBase*>* list, BehaviorData* data);
	//ONOFF選択
	NodeBase* SelectOnOff(std::vector<NodeBase*>* list);



};