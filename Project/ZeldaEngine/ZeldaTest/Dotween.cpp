#include "Dotween.h"
#include "Transform.h"
#include "DotweenTimer.h"
#include <cmath>
#include <float.h>
#include "DotweenTimerPool.h"

using namespace ZeldaEngine;
using namespace ZeldaEngine::ZeldaMath;

Dotween::Dotween()
{
}

Dotween::~Dotween()
{
	// vector 내의 동적할당한 메모리 해제.


	//delete tempTimer;
}

void Dotween::Awake()
{

}

void Dotween::Update()
{
	for (auto itr = m_dotweenTimerMap.begin(); itr != m_dotweenTimerMap.end();)
	{
		itr->first->Update();

		if (itr->first->isDone == true)
		{
			DotweenTimerPool::GetInstance()->ReturnDotweenTimer(itr->first);
			itr = m_dotweenTimerMap.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	//for (const auto& pair : m_dotweenTimerMap)
	//	pair.first->Update();

	//for (auto e : m_dotweenTimerMap)
	//{
	//	e->first->Update();

	//}
	
	//for (auto e : m_dotweenTimerVector)
	//	e->Update();
}

Dotween& Dotween::DOMove(Vector3D endPosition, double p_duration)
{
	// 	GetGameObject()->GetTransform()는 Component를 갖게 될 GameObject를 리턴한다.
	DotweenTimer* m_doMovetweenTimer = DotweenTimerPool::GetInstance()->GetDotweenTimer();


	// 시작 값 저장
	m_doMovetweenTimer->m_xValue = GetGameObject()->GetTransform()->GetLocalPosition().x;
	m_doMovetweenTimer->m_yValue = GetGameObject()->GetTransform()->GetLocalPosition().y;
	m_doMovetweenTimer->m_zValue = GetGameObject()->GetTransform()->GetLocalPosition().z;

	// 가야 하는 거리
	double distanceX = endPosition.x - GetGameObject()->GetTransform()->GetLocalPosition().x;
	double distanceY = endPosition.y - GetGameObject()->GetTransform()->GetLocalPosition().y;
	double distanceZ = endPosition.z - GetGameObject()->GetTransform()->GetLocalPosition().z;

	m_doMovetweenTimer->Start();
	m_doMovetweenTimer->duration = p_duration;
	m_doMovetweenTimer->onUpdate = [=]()
	{
		// 시간 정규화 : 진행 시간 / 전체 기간
		m_doMovetweenTimer->easeTime = (m_doMovetweenTimer->elapsed - m_doMovetweenTimer->delay) / (m_doMovetweenTimer->duration - m_doMovetweenTimer->delay);

		// 가야하는 거리의 정규화...
		// 진행 거리 / 전체 가야하는 거리
		// 진행 거리 = 시간 * 속력

		double easeWeight = getEasingFunction(m_doMovetweenTimer->m_ease)(m_doMovetweenTimer->easeTime);

		// 아래 세줄 안에 문제가 없다면 내 손에 장을 지짐 - 이윤우
		m_doMovetweenTimer->movingDistanceX = easeWeight * (distanceX);
		m_doMovetweenTimer->movingDistanceY = easeWeight * (distanceY);
		m_doMovetweenTimer->movingDistanceZ = easeWeight * (distanceZ);

		m_doMovetweenTimer->finalXvalue = m_doMovetweenTimer->m_xValue + m_doMovetweenTimer->movingDistanceX;
		m_doMovetweenTimer->finalYvalue = m_doMovetweenTimer->m_yValue + m_doMovetweenTimer->movingDistanceY;
		m_doMovetweenTimer->finalZvalue = m_doMovetweenTimer->m_zValue + m_doMovetweenTimer->movingDistanceZ;

		/// 부동 소수점 나눗셈 연산이 문제인듯... 값이 커질수록 버리는 수가 많아 진다....

		if (m_doMovetweenTimer->easeTime >= 1)
		{
			m_doMovetweenTimer->finalXvalue = endPosition.x;
			m_doMovetweenTimer->finalYvalue = endPosition.y;
			m_doMovetweenTimer->finalZvalue = endPosition.z;
		}


		GetGameObject()->GetTransform()->SetLocalPosition(Vector3D(m_doMovetweenTimer->finalXvalue, m_doMovetweenTimer->finalYvalue, m_doMovetweenTimer->finalZvalue));
	};

	tempTimer = m_doMovetweenTimer;
	m_dotweenTimerMap.insert({ tempTimer, m_doMovetweenTimer->isDone });

	/// 자기 자신을 반환해준다...뒤에 SetDelay, SetEase 등을 위해
	return *this;
}

// DOScale, DoRotate 등의 함수도 제작 예정...
// 결국 Vector3의 값을 변경해주는 거라 금방 만들 수 있다
Dotween& Dotween::DOScale(Vector3D endScale, double p_duration)
{
	DotweenTimer* m_doScaletweenTimer = DotweenTimerPool::GetInstance()->GetDotweenTimer();

	m_doScaletweenTimer->m_xValue = GetGameObject()->GetTransform()->GetLocalScale().x;
	m_doScaletweenTimer->m_yValue = GetGameObject()->GetTransform()->GetLocalScale().y;
	m_doScaletweenTimer->m_zValue = GetGameObject()->GetTransform()->GetLocalScale().z;

	double distanceX = endScale.x - GetGameObject()->GetTransform()->GetLocalScale().x;
	double distanceY = endScale.y - GetGameObject()->GetTransform()->GetLocalScale().y;
	double distanceZ = endScale.z - GetGameObject()->GetTransform()->GetLocalScale().z;

	m_doScaletweenTimer->Start();
	m_doScaletweenTimer->duration = p_duration;
	m_doScaletweenTimer->onUpdate = [=]()
	{
		// 소수끼리의 나눗셈 연산은 정확한 값을 만들어 줘야 한다...
		m_doScaletweenTimer->easeTime = (m_doScaletweenTimer->elapsed - m_doScaletweenTimer->delay) / (m_doScaletweenTimer->duration - m_doScaletweenTimer->delay);

		double easeWeight = getEasingFunction(m_doScaletweenTimer->m_ease)(m_doScaletweenTimer->easeTime);

		// 아래 세줄 안에 문제가 없다면 내 손에 장을 지짐 - 이윤우
		m_doScaletweenTimer->movingDistanceX = easeWeight * (distanceX);
		m_doScaletweenTimer->movingDistanceY = easeWeight * (distanceY);
		m_doScaletweenTimer->movingDistanceZ = easeWeight * (distanceZ);

		m_doScaletweenTimer->finalXvalue = m_doScaletweenTimer->m_xValue + m_doScaletweenTimer->movingDistanceX;
		m_doScaletweenTimer->finalYvalue = m_doScaletweenTimer->m_yValue + m_doScaletweenTimer->movingDistanceY;
		m_doScaletweenTimer->finalZvalue = m_doScaletweenTimer->m_zValue + m_doScaletweenTimer->movingDistanceZ;

		/// 부동 소수점 나눗셈 연산이 문제인듯... 값이 커질수록 버리는 수가 많아 진다....

		if (m_doScaletweenTimer->easeTime >= 1)
		{
			m_doScaletweenTimer->finalXvalue = endScale.x;
			m_doScaletweenTimer->finalYvalue = endScale.y;
			m_doScaletweenTimer->finalZvalue = endScale.z;
		}

		GetGameObject()->GetTransform()->SetLocalScale(Vector3D(m_doScaletweenTimer->finalXvalue, m_doScaletweenTimer->finalYvalue, m_doScaletweenTimer->finalZvalue));
	};

	tempTimer = m_doScaletweenTimer;
	m_dotweenTimerMap.insert({ tempTimer, m_doScaletweenTimer->isDone });

	return *this;
}


Dotween& Dotween::DORotate(Vector3D endRotation, double p_duration)
{
	/// DORotate 만들 때 주의할 점.
	/// 1. 각이 360을 넘어가면 0으로 초기화 해준다. 
	/// 일단 여기까지?
	DotweenTimer* m_doRotatetweenTimer = DotweenTimerPool::GetInstance()->GetDotweenTimer();

	m_doRotatetweenTimer->m_xValue = GetGameObject()->GetTransform()->GetLocalRotation().x;
	m_doRotatetweenTimer->m_yValue = GetGameObject()->GetTransform()->GetLocalRotation().y;
	m_doRotatetweenTimer->m_zValue = GetGameObject()->GetTransform()->GetLocalRotation().z;

	double distanceX = endRotation.x - GetGameObject()->GetTransform()->GetLocalRotation().x;
	double distanceY = endRotation.y - GetGameObject()->GetTransform()->GetLocalRotation().y;
	double distanceZ = endRotation.z - GetGameObject()->GetTransform()->GetLocalRotation().z;

	m_doRotatetweenTimer->Start();
	m_doRotatetweenTimer->duration = p_duration;
	m_doRotatetweenTimer->onUpdate = [=]()
	{
		m_doRotatetweenTimer->easeTime = (m_doRotatetweenTimer->elapsed - m_doRotatetweenTimer->delay) / (m_doRotatetweenTimer->duration - m_doRotatetweenTimer->delay);

		double easeWeight = getEasingFunction(m_doRotatetweenTimer->m_ease)(m_doRotatetweenTimer->easeTime);

		m_doRotatetweenTimer->movingDistanceX = easeWeight * (distanceX);
		m_doRotatetweenTimer->movingDistanceY = easeWeight * (distanceY);
		m_doRotatetweenTimer->movingDistanceZ = easeWeight * (distanceZ);

		m_doRotatetweenTimer->finalXvalue = m_doRotatetweenTimer->m_xValue + m_doRotatetweenTimer->movingDistanceX;
		m_doRotatetweenTimer->finalYvalue = m_doRotatetweenTimer->m_yValue + m_doRotatetweenTimer->movingDistanceY;
		m_doRotatetweenTimer->finalZvalue = m_doRotatetweenTimer->m_zValue + m_doRotatetweenTimer->movingDistanceZ;


		if (m_doRotatetweenTimer->easeTime >= 1)
		{
			m_doRotatetweenTimer->finalXvalue = endRotation.x;
			m_doRotatetweenTimer->finalYvalue = endRotation.y;
			m_doRotatetweenTimer->finalZvalue = endRotation.z;

			AdjustRotation(m_doRotatetweenTimer->finalXvalue);
			AdjustRotation(m_doRotatetweenTimer->finalYvalue);
			AdjustRotation(m_doRotatetweenTimer->finalZvalue);
		}

		// 각이 360을 넘어가면 0으로 초기화 해준다.
		// SetRotation 직전에 바꿔주기
		if (m_doRotatetweenTimer->finalXvalue >= 360)
			AdjustRotation(m_doRotatetweenTimer->finalXvalue);

		if (m_doRotatetweenTimer->finalYvalue >= 360)
			AdjustRotation(m_doRotatetweenTimer->finalYvalue);

		if (m_doRotatetweenTimer->finalZvalue >= 360)
			AdjustRotation(m_doRotatetweenTimer->finalZvalue);

		/// 반대로 각도가 -360 미만이라면 -360 ~ 0으로 바꿔준다.
		if (m_doRotatetweenTimer->finalXvalue < 0)
			AdjustRotation(m_doRotatetweenTimer->finalXvalue);

		if (m_doRotatetweenTimer->finalYvalue < 0)
			AdjustRotation(m_doRotatetweenTimer->finalYvalue);

		if (m_doRotatetweenTimer->finalZvalue < 0)
			AdjustRotation(m_doRotatetweenTimer->finalZvalue);

		GetGameObject()->GetTransform()->SetLocalRotation(Vector3D(m_doRotatetweenTimer->finalXvalue, m_doRotatetweenTimer->finalYvalue, m_doRotatetweenTimer->finalZvalue));
	};

	tempTimer = m_doRotatetweenTimer;
	m_dotweenTimerMap.insert({ tempTimer, m_doRotatetweenTimer->isDone });

	return *this;
}

Dotween& Dotween::DOQRotate(Vector3D axis, double angle, double p_duration)
{
	DotweenTimer* m_doQrotatetweenTimer = DotweenTimerPool::GetInstance()->GetDotweenTimer();

	m_doQrotatetweenTimer->Start();
	m_doQrotatetweenTimer->duration = p_duration;
	m_doQrotatetweenTimer->onUpdate = [=]() 
	{
		// 결국 돌려야 하는건 angle, 그런데 ease를 적용한...
		m_doQrotatetweenTimer->easeTime = (m_doQrotatetweenTimer->elapsed - m_doQrotatetweenTimer->delay) / (m_doQrotatetweenTimer->duration - m_doQrotatetweenTimer->delay);
		double easeWeight = getEasingFunction(m_doQrotatetweenTimer->m_ease)(m_doQrotatetweenTimer->easeTime);

		//double anglePerFrame = angle / (p_duration);

		previousAngle = m_doQrotatetweenTimer->movingDistanceX;

		m_doQrotatetweenTimer->movingDistanceX = easeWeight * angle;

		GetGameObject()->GetTransform()->Rotate(axis, DegreeToRadian(m_doQrotatetweenTimer->movingDistanceX - previousAngle));
	};

	tempTimer = m_doQrotatetweenTimer;
	m_dotweenTimerMap.insert({ tempTimer, m_doQrotatetweenTimer->isDone });

	return *this;
}

/// <summary>
/// DOLookAt 함수는 y축을 고려하지 않았다. (위 아래로 회전하지 않는다.)
/// </summary>
/// <param name="lookPosition"></param>
/// <returns></returns>
Dotween& Dotween::DOLookAt(Transform* lookTransform, double p_duration, bool isYaxisInclude)
{	
	DotweenTimer* m_doLookTweenTimer = DotweenTimerPool::GetInstance()->GetDotweenTimer();

	Vector3D objectFront = GetGameObject()->GetTransform()->GetFront();
	Vector3D objectUp;
	Vector3D objectRight = GetGameObject()->GetTransform()->GetRight();
	Vector3D objectLeft = Vector3D(-1 * objectRight.x, objectRight.y, -1 * objectRight.z);

	Vector3D tempLookPos = lookTransform->GetLocalPosition();
	Vector3D tempPos = GetGameObject()->GetTransform()->GetLocalPosition();
	Vector3D tempUp = GetGameObject()->GetTransform()->GetUp();

	Vector3D distanceVec = Vector3D(tempLookPos.x - tempPos.x, tempLookPos.y - tempPos.y, tempLookPos.z - tempPos.z);

	double angle;
	double sq;
	double finalAngle;
	double finalDegree;

	double dot = DotProduct(GetGameObject()->GetTransform()->GetFront(), GetGameObject()->GetTransform()->GetLocalPosition() - tempLookPos);

	// 회전 방향 판정
	Vector3D axis = CrossProduct(objectFront, distanceVec);

	angle = (objectFront.x * distanceVec.x + objectFront.z * distanceVec.z);
	sq = (sqrt(pow(objectFront.x, 2) + pow(objectFront.z, 2)) *
		sqrt(pow(distanceVec.x, 2) + pow(distanceVec.z, 2)));

	// 두 벡터의 각도가 180도 이상이면 180을, -180 이하 이라면 -180을 
	//finalAngle = acos( max( -1.0f, min(1.0f, angle / sq) ) );
	finalAngle = acos(std::clamp(angle / sq, -1.0, 1.0));			// c++17 된다면
	finalDegree = RadianToDegree(finalAngle);

	if (axis.y < 0)
		finalDegree *= -1;

	objectUp = GetGameObject()->GetTransform()->GetUp();

	m_doLookTweenTimer->Start();
	m_doLookTweenTimer->duration = p_duration;
	m_doLookTweenTimer->onUpdate = [=]()
	{
		double degreePerFrame = finalDegree / (m_doLookTweenTimer->duration);
		if (tempPos - tempLookPos != Vector3D(0,0,0) && !isnan(degreePerFrame))
			GetGameObject()->GetTransform()->Rotate(objectUp, DegreeToRadian(degreePerFrame * ZeldaEngine::TimeController::GetInstance().GetDeltaTime()));
	};

	tempTimer = m_doLookTweenTimer;
	m_dotweenTimerMap.insert({ tempTimer, m_doLookTweenTimer->isDone });

	return *this;
}


/// <summary>
/// dotweenTimer의 OnUpdate를 직접 설정해주고 실행가능한 함수.
/// 그럴려면 꼭! OnUpdate를 넣어줘야한다!
/// </summary>
/// <param name="p_duration"></param>
/// <returns></returns>
Dotween& Dotween::DOCustom(double p_duration)
{
	DotweenTimer* m_doCustomTweenTimer = DotweenTimerPool::GetInstance()->GetDotweenTimer();
	m_doCustomTweenTimer->Start();
	m_doCustomTweenTimer->duration = p_duration;

	tempTimer = m_doCustomTweenTimer;
	m_dotweenTimerMap.insert({ tempTimer, m_doCustomTweenTimer->isDone });
	return *this;
}

Dotween& Dotween::DONothing(double p_duration)
{
	DotweenTimer* m_doNothingTweenTimer = DotweenTimerPool::GetInstance()->GetDotweenTimer();
	m_doNothingTweenTimer->Start();
	m_doNothingTweenTimer->duration = p_duration;

	tempTimer = m_doNothingTweenTimer;
	m_dotweenTimerMap.insert({ tempTimer, m_doNothingTweenTimer->isDone });
	return *this;

}

Dotween& Dotween::DOShakePosition(double p_duration, double strength /*= 1.5f*/, int vibrato /*= 1.5*/)
{
	DotweenTimer* m_doShaketweenTimer = DotweenTimerPool::GetInstance()->GetDotweenTimer();
	m_doShaketweenTimer->Start();
	m_doShaketweenTimer->duration = p_duration;
	randPos = Vector3D::zero;

	m_doShaketweenTimer->onUpdate = [=]()
	{
		if (TimeController::GetInstance().GetDeltaTime() != 0)
		{
			GetGameObject()->GetTransform()->SetLocalPosition(GetGameObject()->GetTransform()->GetLocalPosition() - randPos);

			int plusORminus = rand() % 2;

			randRange = Vector3D(rand() % vibrato, rand() % vibrato, 0.0f);

			randRange *= strength;

			switch (plusORminus)
			{
				case 0:
					randPos = randRange;
					break;

				case 1:
					randPos = Vector3D(randRange.x * -1, randRange.y * -1, 0);
					break;
			}

			GetGameObject()->GetTransform()->SetLocalPosition(GetGameObject()->GetTransform()->GetLocalPosition() + randPos);
		}
	};
	
	m_doShaketweenTimer->onExpiration = [=]()
	{
		GetGameObject()->GetTransform()->SetLocalPosition(GetGameObject()->GetTransform()->GetLocalPosition() - randPos);
	};

	tempTimer = m_doShaketweenTimer;
	m_dotweenTimerMap.insert({ tempTimer, m_doShaketweenTimer->isDone });

	return *this;
}

Dotween& Dotween::SetDelay(double p_delay)
{
	tempTimer->delay = p_delay;
	tempTimer->duration += p_delay;

	return *this;
}

Dotween& Dotween::IsRepeat(bool p_repeat)
{
	tempTimer->isRepeat = p_repeat;
	return *this;
}

Dotween& Dotween::SetEase(easing_functions p_ease)
{
	tempTimer->m_ease = p_ease;

	return *this;
}

Dotween& Dotween::OnComplete(std::function<void()> expirationFunc)
{
	tempTimer->onCompleteFunction = expirationFunc;
	return *this;
}

Dotween& Dotween::OnUpdate(std::function<void()> updateFunc)
{
	tempTimer->onUpdate = updateFunc;
	return *this;
}

Dotween& Dotween::InitDotweenTimer()
{
	tempTimer->m_xValue = 0;
	tempTimer->m_yValue = 0;
	tempTimer->m_zValue = 0;
	tempTimer->easeTime = 0;
	tempTimer->elapsed = 0;
	tempTimer->duration = 0;
	tempTimer->delay = 0;
	tempTimer->movingDistanceX = 0;
	tempTimer->movingDistanceY = 0;
	tempTimer->movingDistanceZ = 0;
	tempTimer->speed = 0;
	tempTimer->normalizedDistanceX = 0;
	tempTimer->normalizedDistanceY = 0;
	tempTimer->normalizedDistanceZ = 0;
	tempTimer->finalXvalue = 0;
	tempTimer->finalYvalue = 0;
	tempTimer->finalZvalue = 0;
	tempTimer->accumulated = 0;
	tempTimer->isActive = 0;
	tempTimer->isRepeat = 0;
	tempTimer->isDone = 0;
	tempTimer->onCompleteFunction = []() {};
	tempTimer->onExpiration = []() {};
	tempTimer->onUpdate = []() {};
	return *this;
}

double Dotween::AdjustRotation(double& rot)
{
	while (rot > 360 || rot < 0)
	{
		if (rot < 0)
			rot += 360;
		else if (rot > 360)
			rot -= 360;
	}
	return rot;
}


void Dotween::clearDotweenTimerMap()
{
	if (tempTimer != nullptr)
	{
		tempTimer->onCompleteFunction = []() {};
		tempTimer->onExpiration = []() {};
		tempTimer->onUpdate = []() {};
	}

	auto it = m_dotweenTimerMap.begin();
	while (it != m_dotweenTimerMap.end())
	{
		it->first->onUpdate = []() {};
		it->first->onExpiration = []() {};
		it->first->onCompleteFunction = []() {};
		it++;
	}
}

void Dotween::PreSerialize(json& jsonData) const
{
	PRESERIALIZE_BASE(Dotween);
}

void Dotween::PreDeserialize(const json& jsonData)
{
	PREDESERIALIZE_BASE(Dotween);
}

void Dotween::PostSerialize(json& jsonData) const
{
	POSTSERIALIZE_BASE();
}

void Dotween::PostDeserialize(const json& jsonData)
{
	POSTDESERIALIZE_BASE();
}
