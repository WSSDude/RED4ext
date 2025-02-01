#include "stdafx.hpp"

#include "ShutdownState.hpp"

#include "App.hpp"
#include "GameStateHook.hpp"

namespace
{
GameStateHook<RED4ext::CShutdownState> CShutdownState(&States::ShutdownState::OnEnter, &States::ShutdownState::OnTick,
                                                      &States::ShutdownState::OnExit);
}

bool States::ShutdownState::OnEnter(RED4ext::CShutdownState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CShutdownState, StateSystemAction::Enter, aThis, aApp);
}

bool States::ShutdownState::OnTick(RED4ext::CShutdownState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CShutdownState, StateSystemAction::Tick, aThis, aApp);
}

bool States::ShutdownState::OnExit(RED4ext::CShutdownState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CShutdownState, StateSystemAction::Exit, aThis, aApp);
}

bool States::ShutdownState::Attach(RED4ext::CShutdownState* aState)
{
    return CShutdownState.AttachAt(aState);
}

bool States::ShutdownState::Detach(RED4ext::CShutdownState* aState)
{
    return CShutdownState.DetachAt(aState);
}
