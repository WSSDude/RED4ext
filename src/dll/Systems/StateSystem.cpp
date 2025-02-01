#include "stdafx.hpp"

#include "StateSystem.hpp"

ESystemType StateSystem::GetType()
{
    return ESystemType::State;
}

void StateSystem::Startup()
{
}

void StateSystem::Shutdown()
{
    spdlog::trace("Removing all game state hooks...");

    m_baseInitialization.Shutdown();
    m_initialization.Shutdown();
    m_running.Shutdown();
    m_shutdown.Shutdown();

    spdlog::trace("All game state hooks were removed successfully");
}

bool StateSystem::AddHook(std::shared_ptr<PluginBase> aPlugin, RED4ext::EGameStateType aStateType,
                          RED4ext::GameState* aState)
{
    assert(aState);

    State* state = GetStateByType(aStateType);
    if (!state)
    {
        spdlog::warn(L"The request to add a game state hook for '{}' has failed", aPlugin->GetName());
        return false;
    }

    if (aState->OnBeforeEnter)
    {
        state->onEnter.onBefore.emplace_back(aPlugin, aState->OnBeforeEnter);
    }

    if (aState->OnAfterEnter)
    {
        state->onEnter.onAfter.emplace_back(aPlugin, aState->OnAfterEnter);
    }

    if (aState->OnBeforeTick)
    {
        state->onTick.onBefore.emplace_back(aPlugin, aState->OnBeforeTick);
    }

    if (aState->OnAfterTick)
    {
        state->onTick.onAfter.emplace_back(aPlugin, aState->OnAfterTick);
    }

    if (aState->OnBeforeExit)
    {
        state->onExit.onBefore.emplace_back(aPlugin, aState->OnBeforeExit);
    }

    if (aState->OnAfterExit)
    {
        state->onExit.onAfter.emplace_back(aPlugin, aState->OnAfterExit);
    }

    spdlog::trace(L"The request to add a '{}' state hook for '{}' has been successfully completed", state->GetName(),
                  aPlugin->GetName());
    return true;
}

std::wstring_view StateSystem::StateAction::GetName() const
{
    return name;
}

void StateSystem::StateAction::Shutdown()
{
    onBefore.clear();
    onAfter.clear();
}

std::wstring_view StateSystem::State::GetName() const
{
    return name;
}

void StateSystem::State::Shutdown()
{
    onEnter.Shutdown();
    onTick.Shutdown();
    onExit.Shutdown();
}

StateSystem::State* StateSystem::GetStateByType(RED4ext::EGameStateType aType)
{
    switch (aType)
    {
    case RED4ext::EGameStateType::BaseInitialization:
    {
        return &m_baseInitialization;
    }
    case RED4ext::EGameStateType::Initialization:
    {
        return &m_initialization;
    }
    case RED4ext::EGameStateType::Running:
    {
        return &m_running;
    }
    case RED4ext::EGameStateType::Shutdown:
    {
        return &m_shutdown;
    }
    default:
    {
        spdlog::warn("State with type {} is not handled",
                     static_cast<std::underlying_type_t<RED4ext::EGameStateType>>(aType));
        return nullptr;
    }
    }
}

std::pair<StateSystem::State*, StateSystem::StateAction*> StateSystem::GetStateAndActionByTypes(
    const RED4ext::EGameStateType aType, const StateSystemAction aAction)
{
    static const auto getStateAction = [](auto* aState, const auto aType) -> StateAction*
    {
        switch (aType)
        {
        case StateSystemAction::Enter:
        {
            return &aState->onEnter;
        }
        case StateSystemAction::Tick:
        {
            return &aState->onTick;
        }
        case StateSystemAction::Exit:
        {
            return &aState->onExit;
        }
        default:
        {
            spdlog::warn("StateAction with type {} is not handled",
                         static_cast<std::underlying_type_t<StateSystemAction>>(aType));
            return nullptr;
        }
        }
    };

    if (auto* state = GetStateByType(aType))
    {
        return {state, getStateAction(state, aAction)};
    }

    return {nullptr, nullptr};
}

void StateSystem::Run(std::wstring_view aAction, std::vector<StateItem>& aList, RED4ext::CGameApplication* aApp)
{
    for (auto it = aList.begin(); it != aList.end();)
    {
        const std::wstring_view pluginName = it->plugin ? it->plugin->GetName() : L"RED4ext";

        try
        {
            if (it->func(aApp))
            {
                spdlog::trace(L"Finished running '{}' registered by '{}'", aAction, pluginName);
                it = aList.erase(it);
            }
            else
            {
                ++it;
            }
        }
        catch (const std::exception& e)
        {
            spdlog::warn(L"An exception occured while executing '{}' registered by '{}'", aAction, pluginName);
            spdlog::warn(e.what());
        }
        catch (...)
        {
            spdlog::warn(L"An unknown exception occured while executing '{}' registered by '{}'", aAction, pluginName);
        }
    }
}
