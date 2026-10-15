#include "./Judge/JudgementBase.h"
#include "NodeBase.h"
#include "BehaviorData.h"
#include "../Character/Enemy/EnemyBase.h"

BehaviorTree::BehaviorTree()
{
}
//ノードの追加
void BehaviorTree::AddNode(std::string searchName, std::string entryName, int priority, SelectRule selectRule, JudgementBase* judgment, ActionBase* action,bool OnOff)
{
	if (searchName != "")
	{
		NodeBase* serchNode = root->SearchNode(searchName);

		if (serchNode != NULL)
		{
			//兄弟のNodeBaseの算出
			NodeBase* sibling = root->SearchNode(searchName);
			shared_ptr<NodeBase> addNode = make_shared<NodeBase>(entryName, serchNode, sibling, priority, selectRule, judgment, action, 0, OnOff);
			serchNode->AddChild(addNode);
		}
	}
	else
	{
		if (root == nullptr)
		{
			//最初のノード
			root = make_shared<NodeBase>(entryName, nullptr, nullptr, priority, selectRule, judgment, action, 0);
		}
		
	}
}
// 実行ノードを推論する
NodeBase* BehaviorTree::ActiveNodeInference(EnemyBase* enemy, BehaviorData* data)
{
	data->Init();
	return root->Inference(data);
}
NodeBase* BehaviorTree::Run(EnemyBase* enemy, NodeBase* actionNode, BehaviorData* data)
{
	
	ActionBase::State state = actionNode->Run(enemy);


	// 正常終了
	if (state == ActionBase::State::COMPLETE)
	{
		// シーケンスの途中かを判断
		NodeBase* sequenceNode = data->PopSequenceNode();

		// 途中じゃないなら終了
		if (sequenceNode == NULL)
		{
			return NULL;
		}
		else {
			// 途中ならそこから始める

			actionNode->NodeActionEnd();
			return SequenceBack(sequenceNode, enemy, data);
		}
		// 失敗は終了
	}
	//実行失敗
	else if (state == ActionBase::State::FAILED
		) {
		//実行終了処理
		actionNode->NodeActionEnd();
		return NULL;
	}

	// 現状維持
	return actionNode;
}
// ノード全削除
void BehaviorTree::NodeAllClear(NodeBase* delNode)
{
	for (shared_ptr<NodeBase> node : delNode->children)
	{
		NodeAllClear(node.get());
	}
	delNode->children.clear();

}

// シーケンスノードからの推論開始
NodeBase* BehaviorTree::SequenceBack(NodeBase* sequenceNode, EnemyBase* enemy, BehaviorData* data)
{
	return sequenceNode->Inference(data);
}
