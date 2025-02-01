#include "stdafx.hpp"

#include "BaseInitializationState.hpp"

#include "App.hpp"
#include "GameStateHook.hpp"

namespace
{
GameStateHook<RED4ext::CBaseInitializationState> CBaseInitializationState(&States::BaseInitializationState::OnEnter,
                                                                          &States::BaseInitializationState::OnTick,
                                                                          &States::BaseInitializationState::OnExit);
}

bool States::BaseInitializationState::OnEnter(RED4ext::CBaseInitializationState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CBaseInitializationState, StateSystemAction::Enter, aThis, aApp);
}

bool States::BaseInitializationState::OnTick(RED4ext::CBaseInitializationState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CBaseInitializationState, StateSystemAction::Tick, aThis, aApp);
}

bool States::BaseInitializationState::OnExit(RED4ext::CBaseInitializationState* aThis, RED4ext::CGameApplication* aApp)
{
    return App::Get()->GetStateSystem()->Wrap(CBaseInitializationState, StateSystemAction::Exit, aThis, aApp);
}

bool States::BaseInitializationState::Attach(RED4ext::CBaseInitializationState* aState)
{
    return CBaseInitializationState.AttachAt(aState);
}

bool States::BaseInitializationState::Detach(RED4ext::CBaseInitializationState* aState)
{
    return CBaseInitializationState.DetachAt(aState);
}
