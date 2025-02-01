#include "stdafx.hpp"

#include "InitializationState.hpp"

#include "App.hpp"
#include "GameStateHook.hpp"

namespace
{
GameStateHook<RED4ext::CInitializationState> CInitializationState(&States::InitializationState::OnEnter,
                                                                  &States::InitializationState::OnTick,
                                                                  &States::InitializationState::OnExit);
}

bool States::InitializationState::OnEnter(RED4ext::CInitializationState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CInitializationState, StateSystemAction::Enter, aThis, aApp);
}

bool States::InitializationState::OnTick(RED4ext::CInitializationState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CInitializationState, StateSystemAction::Tick, aThis, aApp);
}

bool States::InitializationState::OnExit(RED4ext::CInitializationState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CInitializationState, StateSystemAction::Exit, aThis, aApp);
}

bool States::InitializationState::Attach(RED4ext::CInitializationState* aState)
{
    return CInitializationState.AttachAt(aState);
}

bool States::InitializationState::Detach(RED4ext::CInitializationState* aState)
{
    return CInitializationState.DetachAt(aState);
}
