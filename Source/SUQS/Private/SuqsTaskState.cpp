#include "SuqsTaskState.h"

#include <algorithm>

#include "SuqsPlayState.h"

void USuqsTaskState::Initialise(const FSuqsTask* TaskDef, USuqsObjectiveState* ObjState, USuqsPlayState* Root)
{
	TaskDefinition = TaskDef;
	ParentObjective = ObjState;
	PlayState = Root;
}

void USuqsTaskState::Tick(float DeltaTime)
{
	if (IsTimeLimited() && TimeRemaining > 0)
	{
		TimeRemaining -= DeltaTime;
		if (TimeRemaining <= 0)
		{
			TimeRemaining = 0;
			Fail();
		}
	}
	
}

void USuqsTaskState::ChangeStatus(ESuqsTaskStatus NewStatus)
{
	if (Status != NewStatus)
	{
		Status = NewStatus;

		switch(NewStatus)
		{
		case ESuqsTaskStatus::Completed: 
			PlayState->RaiseTaskCompleted(this);
			break;
		case ESuqsTaskStatus::Failed:
			PlayState->RaiseTaskFailed(this);
			break;
		default: break;
		}

		ParentObjective->NotifyTaskStatusChanged();
		PlayState->RaiseQuestUpdated(this->GetParentObjective()->GetParentQuest());
		
	}
}


void USuqsTaskState::Fail()
{
	ChangeStatus(ESuqsTaskStatus::Failed);
}

void USuqsTaskState::Complete()
{
	Number = TaskDefinition->TargetNumber;
	ChangeStatus(ESuqsTaskStatus::Completed);
}

void USuqsTaskState::Progress(int Delta)
{
	Number += Delta;
	Number = std::min(std::max(0, Number), TaskDefinition->TargetNumber);
	if (Number == TaskDefinition->TargetNumber)
		Complete();
}