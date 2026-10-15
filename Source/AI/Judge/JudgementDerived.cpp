#include "JudgementDerived.h"
#include "../Source/Character/Enemy/EnemyBase.h"
//‘Ò‹@”»’èˆ—
bool IdleJudgement::Judgement()
{
	//–³ðŒ‚ÅTRUE
	return true;
}

//’ÇÕ”»’èˆ—
bool PursuitJudgement::Judgement()
{
	//õ“G“à‚É‚¢‚ê‚Îtrue‚ð•Ô‚·
	if (owner->SearchPosition(owner->GetSearchLength(), owner->GetTarget()->GetPos()))
	{
		return true;
	}
	return false;
}

//UŒ‚”»’èˆ—
bool AttackJudgement::Judgement()
{
	//õ“G“à‚É‚¢‚ê‚Îtrue‚ð•Ô‚·
	if (owner->SearchPosition(owner->GetAttackLength(), owner->GetTarget()->GetPos()))
	{
		return true;
	}
	return false;
}
//ƒ_ƒ[ƒW”»’èˆ—
bool DamageJudgement::Judgement()
{
	return true;
}

//Ž€–S”»’èˆ—
bool DeadJudgement::Judgement()
{
	if (owner->GetHP() <= 0)
	{
		return true;
	}
	return false;;
}

//‰ñ“]UŒ‚”»’èˆ—iƒ{ƒXê—pj
bool BossRollAttackJudgement::Judgement()
{
	return true;
}