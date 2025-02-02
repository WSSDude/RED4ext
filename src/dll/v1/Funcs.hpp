#pragma once

namespace v1
{
namespace Hooking
{
bool Attach(RED4ext::PluginHandle aHandle, void* aTarget, void* aDetour, void** aOriginal);
bool Detach(RED4ext::PluginHandle aHandle, void* aTarget);
} // namespace Hooking

namespace GameStates
{
bool AddHook(RED4ext::PluginHandle aHandle, RED4ext::EGameStateType aType, const RED4ext::GameState& aState);
} // namespace GameStates

namespace Scripts
{
bool Add(RED4ext::PluginHandle aHandle, std::wstring_view aPath);
} // namespace Scripts

namespace Rendering
{
bool AddHook(RED4ext::PluginHandle aHandle, const RED4ext::Render& aRender);
} // namespace Rendering

} // namespace v1
