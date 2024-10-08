#pragma once
#include <iostream>
#include <functional>
#include <unordered_map>
#include <vector>


/// <summary>
/// FSM
/// 
/// 오브젝트마다 상태를 전환할때 사용
/// 템플릿으로 만들어 각 오브젝트에 상속적인 클래스가 아니라
/// 멤버변수로 넣어 범용적으로 사용가능하다.
/// 
/// 필요한 오브젝트에 상태를 받아
/// </summary>
/// <typeparam name="StateEnum"></typeparam>
template <typename StateEnum>
class FSM
{
public:
	typedef std::function<bool()> Condition;
	typedef std::function<void()> Action;
	typedef std::vector<std::pair<StateEnum, Condition>> TransitionList;

	// 상태 전환
	std::unordered_map<StateEnum, TransitionList> transitions;
	// 상태 진입
	std::unordered_map<StateEnum, Action> engageAction;
	// 상태 유지
	std::unordered_map<StateEnum, Action> updateAction;

	// 현재상태 enum
	StateEnum currentState;
	// 이전상태 enum
	StateEnum previousState;

	// 생성자
	// 매개변수로 최초 상태를 받아 초기화해둔다.
	FSM(StateEnum initialState)
		: currentState(initialState), previousState(initialState)
	{
		// typename으로 받은 StateEnum 안의 모든 enum의 액션을 초기화해준다.
		for (unsigned int state = 0; state < static_cast<unsigned int>(StateEnum::StateEnd); state++)
		{
			transitions[static_cast<StateEnum>(state)] = TransitionList();
			engageAction[static_cast<StateEnum>(state)] = []() {};
			updateAction[static_cast<StateEnum>(state)] = []() {};
		}
	}

	// update()에 넣어서 주기적으로 계속 업데이트 해준다.
	void UpdateState();
};

template <typename StateEnum>
void FSM<StateEnum>::UpdateState()
{
	bool engaged = false;
	auto& transitionList = transitions[currentState];

	for (auto eachTransition : transitionList)
	{
		// TransitionList의 pair에서 두번째 bool함수가 true일 경우 상태를 바꾼다.
		if (eachTransition.second())
		{
			previousState = currentState;
			currentState = eachTransition.first;
			engaged = true;
			break;
		}
	}

	// 바꾼상태로 진입할 때
	if (engaged == true)
	{
		engageAction[currentState]();
	}

	// 상태가 유지될 때
	updateAction[currentState]();
}

