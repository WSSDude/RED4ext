#include "stdafx.hpp"

#include "RunningState.hpp"

#include "App.hpp"
#include "GameStateHook.hpp"

namespace
{
GameStateHook<RED4ext::CRunningState> CRunningState(&States::RunningState::OnEnter, &States::RunningState::OnTick,
                                                    &States::RunningState::OnExit);
}

bool States::RunningState::OnEnter(RED4ext::CRunningState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CRunningState, StateSystemAction::Enter, aThis, aApp);
}

bool States::RunningState::OnTick(RED4ext::CRunningState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CRunningState, StateSystemAction::Tick, aThis, aApp);
}

bool States::RunningState::OnExit(RED4ext::CRunningState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CRunningState, StateSystemAction::Exit, aThis, aApp);
}

bool States::RunningState::Attach(RED4ext::CRunningState* aState)
{
    return CRunningState.AttachAt(aState);
}

bool States::RunningState::Detach(RED4ext::CRunningState* aState)
{
    return CRunningState.DetachAt(aState);
}
