#include "NodeBase.h"
#include "BehaviorData.h"
#include "../Character/Enemy/EnemyBase.h"
#include <assert.h>

// ノード検索
NodeBase* NodeBase::SearchNode(std::string searchName)
{
	if (name == searchName)return this;

	//一応　子供クラスでも検索をかける
	for (int i = 0; i < children.size(); i++)
	{
		NodeBase* child = GetChild(i)->SearchNode(searchName);

		if (child == nullptr)continue;

		if (child->name == searchName)
		{
			return child;
		}
	}

	assert("検索にヒットしなかった");
	return nullptr;
}

//ノードのアクション
ActionBase::State NodeBase::Run(EnemyBase* enemy)
{
	if (enemy)
	{
		return action->Run();
	}
	
		//次の行動に移行！
	else 	return ActionBase::State::FAILED;

	return ActionBase::State::FAILED;
}

//ノード推論
NodeBase* NodeBase::Inference(BehaviorData* data)
{
	
	std::vector<NodeBase*> list;
	list.clear();
	NodeBase* result = nullptr;

	
	//子ノードで実行可能なノードを探す
	
	
	for (int i = 0; i < children.size(); i++)
	{
		if (children.at(i)->judgement != nullptr)
		{
			if (children.at(i)->judgement->Judgement())
			{
				list.emplace_back(children.at(i).get());
			}
		}
		else {
			// 判定クラスがなければ無条件に追加
			list.emplace_back(children.at(i).get());
		}
	}

	//ルールの分岐
	switch (selectRule)
	{
		//優先順位
	case BehaviorTree::SelectRule::PRIORITY:
		result=SelectPriority(&list);
		break;
		//ランダム
	case BehaviorTree::SelectRule::RANDOM:
		result = SelectRandom(&list);
		break;
		//シーケンス
	case BehaviorTree::SelectRule::SEQUENCE:
		result = SelectSequence(&list, data);
		break;
		//ONOFF
	case BehaviorTree::SelectRule::ON_OFF:
		result = SelectOnOff(&list);
		break;
	}
	if (result!=nullptr)
	{
		//行動があるなら終了処理
		if (result->action != nullptr)
		{
			//行動開始処理
			result->NodeActionStart();
			return result;
		}
		else
		{
			return result = result->Inference(data);
		}



	}
	return result;

}
//ノードのアクション
//bool NodeBase::Run()
//{
//	ActionBase::State state=
//}
//ランダムに一つ選択する
NodeBase* NodeBase::SelectRandom(std::vector<NodeBase*> *list)
{
	int rnd = rand() % list->size();
	return list->at(rnd);
}
//優先順位で選択する
NodeBase* NodeBase::SelectPriority(std::vector<NodeBase*> *list)
{
	int MaxPriority = INT_MAX;
	NodeBase* SelectNode=nullptr;

	for (int i = 0; i < list->size(); i++)
	{
		if (MaxPriority >= list->at(i)->GetPriority())
		{
			SelectNode=list->at(i);
			MaxPriority = list->at(i)->GetPriority();
		}
		
	}
	return SelectNode;
}

// シーケンス
NodeBase* NodeBase::SelectSequence(std::vector<NodeBase*>* list, BehaviorData* data)
{
	
	int step = 0;
	step = data->GetSequenceStep(GetName());

	if (step >= (int)children.size())
	{
		if (selectRule == BehaviorTree::SelectRule::SEQUETIAL_LOOPING)step = 0;
		else return NULL;
		
	}


	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
	
		if (children.at(step)->GetName() == (*itr)->GetName())
		{

		
			data->PushSequenceNode(this);
		
			data->SetSequenceStep(GetName(), step + 1);

			return children.at(step).get();
		}
	}

	return NULL;
}

//一度も選択されたことがない場合　それを選択する
//しかしすべて一通り選択されたことがあるならSelectPriority()で選択する。
NodeBase* NodeBase::SelectOnOff(std::vector<NodeBase*>* list)
{
	int MaxPriority = INT_MAX;
	NodeBase* SelectNode = nullptr;

	for (int i = 0; i < list->size(); i++)
	{
		//選択されたことがない場合
		if (list->at(i)->oldSelect==false)
		{
			list->at(i)->oldSelect = true;
			return list->at(i);
		}


		if (MaxPriority >= list->at(i)->GetPriority())
		{
			SelectNode = list->at(i);
			MaxPriority = list->at(i)->GetPriority();
		}

	}
	return SelectNode;
}
