#pragma once

#include <vector>
#include <stack>
#include <map>
#include "BehaviorTree.h"

class NodeBase;

// Behavior保存データ
class BehaviorData
{
public:
	// コンストラクタ
	BehaviorData() { Init(); }
	// シーケンスノードのプッシュ
	void PushSequenceNode(NodeBase* node) { sequenceStack.push(node); }
	// シーケンスノードのポップ
	NodeBase* PopSequenceNode();
	// シーケンスステップのゲッター
	int GetSequenceStep(const std::string name);
	// シーケンスステップのセッター
	void SetSequenceStep(const std::string name,const int step);
	// 初期化
	void Init();
private:
	// シーケンスノードスタック
	std::stack<NodeBase*> sequenceStack;				
	// 実行シーケンスのステップマップ
	std::map<std::string, int> runSequenceStepMap;		
};

