#pragma once

#include "ISystem.hpp"

#include "GameStateHook.hpp"
#include "PluginBase.hpp"

enum class StateSystemAction : uint8_t
{
    Enter,
    Tick,
    Exit
};

class StateSystem final : public ISystem
{
public:
    ESystemType GetType() final;

    void Startup() final;
    void Shutdown() final;

    bool AddHook(std::shared_ptr<PluginBase> aPlugin, RED4ext::EGameStateType aStateType, RED4ext::GameState* aState);

    template<std::derived_from<RED4ext::IGameState> T>
    bool Wrap(GameStateHook<T>& aHooked, StateSystemAction aAction, T* aThis, RED4ext::CGameApplication* aApp);

private:
    using Func_t = bool (*)(RED4ext::CGameApplication*);

    struct StateItem
    {
        std::shared_ptr<PluginBase> plugin;
        Func_t func;
    };

    struct StateAction
    {
        std::wstring_view GetName() const;

        void Shutdown();

        std::wstring_view name;
        std::vector<StateItem> onBefore;
        std::vector<StateItem> onAfter;
    };

    struct State
    {
        std::wstring_view GetName() const;

        void Shutdown();

        std::wstring_view name;
        StateAction onEnter = {.name = L"Enter"};
        StateAction onTick = {.name = L"Tick"};
        StateAction onExit = {.name = L"Exit"};
    };

    template<std::derived_from<RED4ext::IGameState> T>
    bool ExecuteHookedAction(GameStateHook<T>& aHooked, StateSystemAction aAction, T* aThis,
                             RED4ext::CGameApplication* aApp);

    State* GetStateByType(RED4ext::EGameStateType aType);
    std::pair<State*, StateAction*> GetStateAndActionByTypes(RED4ext::EGameStateType aType, StateSystemAction aAction);

    void Run(std::wstring_view aAction, std::vector<StateItem>& aList, RED4ext::CGameApplication* aApp);

    State m_baseInitialization = {.name = L"BaseInitialization"};
    State m_initialization = {.name = L"Initialization"};
    State m_running = {.name = L"Running"};
    State m_shutdown = {.name = L"Shutdown"};
};

template<std::derived_from<RED4ext::IGameState> T>
inline bool StateSystem::Wrap(GameStateHook<T>& aHooked, const StateSystemAction aAction, T* aThis,
                              RED4ext::CGameApplication* aApp)
{
    auto [state, action] = GetStateAndActionByTypes(aThis->GetType(), aAction);
    if (!state || !action)
        return ExecuteHookedAction(aHooked, aAction, aThis, aApp);

    Run(fmt::format(L"{}::OnBefore{}", state->GetName(), action->GetName()), action->onBefore, aApp);
    const auto result = ExecuteHookedAction(aHooked, aAction, aThis, aApp);
    Run(fmt::format(L"{}::OnAfter{}", state->GetName(), action->GetName()), action->onAfter, aApp);

    return result;
}

template<std::derived_from<RED4ext::IGameState> T>
inline bool StateSystem::ExecuteHookedAction(GameStateHook<T>& aHooked, const StateSystemAction aAction, T* aThis,
                                             RED4ext::CGameApplication* aApp)
{
    switch (aAction)
    {
    case StateSystemAction::Enter:
    {
        return aHooked.OnEnter(aThis, aApp);
    }
    case StateSystemAction::Tick:
    {
        return aHooked.OnTick(aThis, aApp);
    }
    case StateSystemAction::Exit:
    {
        return aHooked.OnExit(aThis, aApp);
    }
    default:
    {
        spdlog::error("StateAction with type {} is not handled",
                      static_cast<std::underlying_type_t<StateSystemAction>>(aAction));
        assert(false);
        return false;
    }
    }
}
