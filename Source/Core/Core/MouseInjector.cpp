#include "Core/MouseInjector.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdio>

#include "Core/ConfigManager.h"
#include "Core/Core.h"
#include "Core/HW/Memmap.h"
#include "Core/System.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace MouseInjector
{
static constexpr u32 TS2_PLAYERBASE = 0x804686CC;
static constexpr u32 TS2_PLAYER1BASE_COOP = 0x807FFC28;
static constexpr u32 TS2_PLAYER2BASE_COOP = 0x807FFEC8;
static constexpr u32 TS2_YAXISLIMIT = 0x804686BC;
static constexpr u32 TS2_FOV = 0x8046818C;
static constexpr u32 TS2_PLAYER_STRUCT_STRIDE = 0xD60;
static constexpr u32 TS2_CAM_X_OFFSET = 0x148;
static constexpr u32 TS2_CAM_Y_OFFSET = 0x14C;
static constexpr u32 TS2_J1_CAMERA_PTR_OFFSET = 0x008;
static constexpr u32 TS2_J1_LOCAL_FOV_OFFSET = 0x324;
static constexpr u32 TS2_CAMERA_MODE_OFFSET = 0x158;
static constexpr u32 TS2_CAMERA_MODE_VALUE = 8;
static constexpr float TS2_DEFAULT_FOV = 60.f;
static constexpr float TS2_MIN_FOV_FACTOR = 0.01f;

static bool s_active = false;
static float s_sensitivity = 0.5f;
static int s_lock_cx = 0;
static int s_lock_cy = 0;

static constexpr float MIN_SENSITIVITY = 0.1f;
static constexpr float MAX_SENSITIVITY = 5.0f;
static constexpr float SENSITIVITY_STEP = 0.1f;

static bool IsTS2()
{
  return SConfig::GetInstance().GetGameID() == "GTSE4F";
}

static bool IsValidPlayerBase(u32 address)
{
  return address >= 0x80000000U && address <= 0x817FF000U;
}

static float ReadF32(Core::System& system, u32 addr)
{
  return std::bit_cast<float>(system.GetMemory().Read_U32(addr));
}

static void WriteF32(Core::System& system, u32 addr, float value)
{
  system.GetMemory().Write_U32(std::bit_cast<u32>(value), addr);
}

static bool IsLikelyTS2PlayerBase(Core::System& system, u32 address)
{
  if (!IsValidPlayerBase(address))
    return false;

  const float camx = ReadF32(system, address + TS2_CAM_X_OFFSET);
  const float camy = ReadF32(system, address + TS2_CAM_Y_OFFSET);
  return std::isfinite(camx) && std::isfinite(camy) && camx >= 0.f && camx < 360.f &&
         camy >= -90.f && camy <= 90.f;
}

static void UpdateLockCenter()
{
#ifdef _WIN32
  HWND hwnd = GetForegroundWindow();
  RECT rect;
  if (GetWindowRect(hwnd, &rect))
  {
    s_lock_cx = (rect.left + rect.right) / 2;
    s_lock_cy = (rect.top + rect.bottom) / 2;
  }
#endif
}

static Delta GetMouseDelta()
{
  Delta delta;
#ifdef _WIN32
  POINT p;
  if (GetCursorPos(&p))
  {
    const int dx = p.x - s_lock_cx;
    const int dy = p.y - s_lock_cy;
    delta.dx = static_cast<float>(dx) * s_sensitivity;
    delta.dy = static_cast<float>(dy) * s_sensitivity;
    SetCursorPos(s_lock_cx, s_lock_cy);
  }
#endif
  return delta;
}

static u32 GetTS2PlayerBase(Core::System& system, int ingame_pad)
{
  const u32 playerbase = system.GetMemory().Read_U32(TS2_PLAYERBASE);
  const u32 player1base_coop = system.GetMemory().Read_U32(TS2_PLAYER1BASE_COOP);
  const u32 player2base_coop = system.GetMemory().Read_U32(TS2_PLAYER2BASE_COOP);

  if (IsValidPlayerBase(player1base_coop) && IsValidPlayerBase(player2base_coop) &&
      player1base_coop != player2base_coop)
  {
    return ingame_pad == 1 ? player2base_coop : player1base_coop;
  }

  const u32 player2base_by_stride = playerbase + TS2_PLAYER_STRUCT_STRIDE;
  if (IsLikelyTS2PlayerBase(system, playerbase) &&
      IsLikelyTS2PlayerBase(system, player2base_by_stride))
  {
    return ingame_pad == 1 ? player2base_by_stride : playerbase;
  }

  return playerbase;
}

static float GetTS2FovForPlayer(Core::System& system, u32 playerbase)
{
  float fov = ReadF32(system, TS2_FOV);
  const u32 camera = system.GetMemory().Read_U32(playerbase + TS2_J1_CAMERA_PTR_OFFSET);
  if (IsValidPlayerBase(camera))
  {
    const float local_fov = ReadF32(system, camera + TS2_J1_LOCAL_FOV_OFFSET);
    if (std::isfinite(local_fov) && local_fov > 3.f && local_fov <= 120.f)
      fov = local_fov;
  }
  return fov;
}

static bool IsTS2CameraMode(Core::System& system, u32 playerbase)
{
  return IsValidPlayerBase(playerbase) &&
         system.GetMemory().Read_U32(playerbase + TS2_CAMERA_MODE_OFFSET) == TS2_CAMERA_MODE_VALUE;
}

static void InjectTS2(Core::System& system, int ingame_pad, Delta delta)
{
  if (delta.dx == 0 && delta.dy == 0)
    return;

  const u32 playerbase = GetTS2PlayerBase(system, ingame_pad);
  if (!IsValidPlayerBase(playerbase))
    return;

  if (IsTS2CameraMode(system, playerbase))
    return;

  const float fov = GetTS2FovForPlayer(system, playerbase);

  const float yaxislimit = ReadF32(system, TS2_YAXISLIMIT);
  if (!std::isfinite(fov) || fov <= 3.f)
    return;

  float camx = ReadF32(system, playerbase + TS2_CAM_X_OFFSET);
  float camy = ReadF32(system, playerbase + TS2_CAM_Y_OFFSET);

  if (!std::isfinite(camx) || !std::isfinite(camy))
    return;

  const float fov_ratio = fov / TS2_DEFAULT_FOV;
  float fov_factor =
      std::clamp(std::min(fov_ratio, 1.f / fov_ratio), TS2_MIN_FOV_FACTOR, 1.f);

  if (camx < 0.f || camx >= 360.f || camy < -yaxislimit || camy > yaxislimit)
    return;

  camx -= delta.dx / 10.f * fov_factor;
  camy -= delta.dy / 10.f * fov_factor;

  while (camx < 0.f)
    camx += 360.f;
  while (camx >= 360.f)
    camx -= 360.f;
  camy = std::clamp(camy, -yaxislimit, yaxislimit);

  WriteF32(system, playerbase + TS2_CAM_X_OFFSET, camx);
  WriteF32(system, playerbase + TS2_CAM_Y_OFFSET, camy);
}

void Toggle()
{
  s_active = !s_active;
  UpdateLockCenter();
  Core::DisplayMessage(s_active ? "NetPlay 2P Mouse: enabled" :
                                  "NetPlay 2P Mouse: disabled",
                       2000);
#ifdef _WIN32
  ShowCursor(!s_active);
#endif
}

bool IsActive()
{
  return s_active;
}

void DecreaseSensitivity()
{
  s_sensitivity = std::clamp(s_sensitivity - SENSITIVITY_STEP, MIN_SENSITIVITY, MAX_SENSITIVITY);
  char message[80];
  std::snprintf(message, sizeof(message), "NetPlay 2P Mouse sensitivity: %.1f", s_sensitivity);
  Core::DisplayMessage(message, 1500);
}

void IncreaseSensitivity()
{
  s_sensitivity = std::clamp(s_sensitivity + SENSITIVITY_STEP, MIN_SENSITIVITY, MAX_SENSITIVITY);
  char message[80];
  std::snprintf(message, sizeof(message), "NetPlay 2P Mouse sensitivity: %.1f", s_sensitivity);
  Core::DisplayMessage(message, 1500);
}

void SetFocus(bool focused)
{
  if (focused && !s_active)
  {
    s_active = true;
    UpdateLockCenter();
#ifdef _WIN32
    ShowCursor(FALSE);
#endif
  }
  else if (!focused && s_active)
  {
    s_active = false;
#ifdef _WIN32
    ShowCursor(TRUE);
#endif
  }
}

Delta CaptureDelta()
{
  auto& system = Core::System::GetInstance();
  if (!Core::IsRunning(system) || !IsTS2())
    return {};

  if (!s_active)
    return {};

  return GetMouseDelta();
}

void ApplyNetPlayDelta(int ingame_pad, const Delta& delta)
{
  auto& system = Core::System::GetInstance();
  if (!Core::IsRunning(system) || !IsTS2())
    return;

  InjectTS2(system, ingame_pad, delta);
}

void Update()
{
  ApplyNetPlayDelta(0, CaptureDelta());
}
}  // namespace MouseInjector
