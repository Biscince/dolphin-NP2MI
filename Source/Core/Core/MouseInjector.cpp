#include "Core/MouseInjector.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdio>

#include "Core/ConfigManager.h"
#include "Core/Core.h"
#include "Core/HW/Memmap.h"
#include "Core/System.h"
#include "InputCommon/GCPadStatus.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace MouseInjector
{
struct TS2MemoryProfile
{
  const char* game_id;
  u32 playerbase;
  u32 player1base_coop;
  u32 player2base_coop;
  u32 yaxislimit;
  u32 fov;
};

static constexpr TS2MemoryProfile TS2_NTSC_U_PROFILE = {
    "GTSE4F", 0x804686CC, 0x807FFC28, 0x807FFEC8, 0x804686BC, 0x8046818C};
static constexpr TS2MemoryProfile TS2_PAL_PROFILE = {
    "GTSP4F", 0x80469E4C, 0x8082E428, 0x8082E6C8, 0x80469E3C, 0x8046990C};
static constexpr u32 TS2_PLAYER_STRUCT_STRIDE = 0xD60;
static constexpr u32 TS2_CAM_X_OFFSET = 0x148;
static constexpr u32 TS2_CAM_Y_OFFSET = 0x14C;
static constexpr u32 TS2_ALT_CAM_X_OFFSET = 0x1AC;
static constexpr u32 TS2_ALT_CAM_Y_OFFSET = 0x1B0;
static constexpr u32 TS2_J1_CAMERA_PTR_OFFSET = 0x008;
static constexpr u32 TS2_J1_LOCAL_FOV_OFFSET = 0x324;
static constexpr u32 TS2_CAMERA_MODE_OFFSET = 0x158;
static constexpr u32 TS2_CAMERA_MODE_VALUE = 8;
static constexpr u32 TS2_AIM_MODE_OFFSET = 0x8A8;
static constexpr u32 TS2_AIM_MODE_VALUE = 1;
static constexpr float TS2_DEFAULT_FOV = 60.f;
static constexpr float TS2_MIN_FOV_FACTOR = 0.01f;
static constexpr float TS2_CAMERA_PAD_HORIZONTAL_SCALE_DEFAULT = 10.0f;
static constexpr float TS2_CAMERA_PAD_VERTICAL_SCALE_DEFAULT = 10.0f;
static constexpr float TS2_CAMERA_PAD_HORIZONTAL_LIMIT_DEFAULT = 56.0f;
static constexpr float TS2_CAMERA_PAD_VERTICAL_LIMIT_DEFAULT = 56.0f;
static constexpr float TS2_CAMERA_PAD_HORIZONTAL_STEP_DEFAULT = 12.0f;
static constexpr float TS2_CAMERA_PAD_VERTICAL_STEP_DEFAULT = 11.0f;
static constexpr float TS2_CAMERA_PAD_HORIZONTAL_ATTACK_DEFAULT = 0.15f;
static constexpr float TS2_CAMERA_PAD_VERTICAL_ATTACK_DEFAULT = 0.15f;
static constexpr float TS2_CAMERA_PAD_DECAY_DEFAULT = 0.55f;
static constexpr float TS2_CAMERA_PAD_HORIZONTAL_ACCUM_DECAY_DEFAULT = 0.82f;
static constexpr float TS2_CAMERA_PAD_VERTICAL_ACCUM_DECAY_DEFAULT = 0.82f;
static constexpr float TS2_CAMERA_AIM_HORIZONTAL_SCALE_DEFAULT = 1.0f;
static constexpr float TS2_CAMERA_AIM_VERTICAL_SCALE_DEFAULT = 1.0f;
static constexpr float TS2_CAMERA_AIM_HORIZONTAL_LIMIT_DEFAULT = 51.0f;
static constexpr float TS2_CAMERA_AIM_VERTICAL_LIMIT_DEFAULT = 51.0f;
static constexpr float TS2_CAMERA_AIM_HORIZONTAL_RESPONSE_CURVE_DEFAULT = 0.25f;
static constexpr float TS2_CAMERA_AIM_VERTICAL_RESPONSE_CURVE_DEFAULT = 0.25f;
static constexpr float TS2_CAMERA_PAD_HORIZONTAL_DIRECTION = 1.0f;
static constexpr float TS2_CAMERA_PAD_VERTICAL_DIRECTION = -1.0f;

static bool s_active = false;
static bool s_camera_mouse_mode = true;
static float s_sensitivity = 0.5f;
static int s_lock_cx = 0;
static int s_lock_cy = 0;
static float s_camera_horizontal_scale = TS2_CAMERA_PAD_HORIZONTAL_SCALE_DEFAULT;
static float s_camera_vertical_scale = TS2_CAMERA_PAD_VERTICAL_SCALE_DEFAULT;
static float s_camera_horizontal_limit = TS2_CAMERA_PAD_HORIZONTAL_LIMIT_DEFAULT;
static float s_camera_vertical_limit = TS2_CAMERA_PAD_VERTICAL_LIMIT_DEFAULT;
static float s_camera_horizontal_step = TS2_CAMERA_PAD_HORIZONTAL_STEP_DEFAULT;
static float s_camera_vertical_step = TS2_CAMERA_PAD_VERTICAL_STEP_DEFAULT;
static float s_camera_horizontal_attack = TS2_CAMERA_PAD_HORIZONTAL_ATTACK_DEFAULT;
static float s_camera_vertical_attack = TS2_CAMERA_PAD_VERTICAL_ATTACK_DEFAULT;
static float s_camera_decay = TS2_CAMERA_PAD_DECAY_DEFAULT;
static float s_camera_horizontal_accum_decay =
    TS2_CAMERA_PAD_HORIZONTAL_ACCUM_DECAY_DEFAULT;
static float s_camera_vertical_accum_decay = TS2_CAMERA_PAD_VERTICAL_ACCUM_DECAY_DEFAULT;
static float s_camera_aim_horizontal_scale = TS2_CAMERA_AIM_HORIZONTAL_SCALE_DEFAULT;
static float s_camera_aim_vertical_scale = TS2_CAMERA_AIM_VERTICAL_SCALE_DEFAULT;
static float s_camera_aim_horizontal_limit = TS2_CAMERA_AIM_HORIZONTAL_LIMIT_DEFAULT;
static float s_camera_aim_vertical_limit = TS2_CAMERA_AIM_VERTICAL_LIMIT_DEFAULT;
static float s_camera_aim_horizontal_response_curve =
    TS2_CAMERA_AIM_HORIZONTAL_RESPONSE_CURVE_DEFAULT;
static float s_camera_aim_vertical_response_curve =
    TS2_CAMERA_AIM_VERTICAL_RESPONSE_CURVE_DEFAULT;
static std::array<float, 4> s_camera_raw_x{};
static std::array<float, 4> s_camera_raw_y{};
static std::array<float, 4> s_camera_pad_x{};
static std::array<float, 4> s_camera_pad_y{};
static std::array<float, 4> s_camera_aim_x{};
static std::array<float, 4> s_camera_aim_y{};
static std::array<bool, 4> s_camera_restricted_aim{};

static constexpr float MIN_SENSITIVITY = 0.1f;
static constexpr float MAX_SENSITIVITY = 5.0f;
static constexpr float SENSITIVITY_STEP = 0.1f;

static void ClearCameraPadState()
{
  s_camera_raw_x.fill(0.0f);
  s_camera_raw_y.fill(0.0f);
  s_camera_pad_x.fill(0.0f);
  s_camera_pad_y.fill(0.0f);
  s_camera_aim_x.fill(0.0f);
  s_camera_aim_y.fill(0.0f);
  s_camera_restricted_aim.fill(false);
}

static void ClearCameraPadState(int ingame_pad)
{
  s_camera_raw_x[ingame_pad] = 0.0f;
  s_camera_raw_y[ingame_pad] = 0.0f;
  s_camera_pad_x[ingame_pad] = 0.0f;
  s_camera_pad_y[ingame_pad] = 0.0f;
  s_camera_aim_x[ingame_pad] = 0.0f;
  s_camera_aim_y[ingame_pad] = 0.0f;
  s_camera_restricted_aim[ingame_pad] = false;
}

static const TS2MemoryProfile* GetTS2Profile()
{
  const std::string& game_id = SConfig::GetInstance().GetGameID();
  if (game_id == TS2_NTSC_U_PROFILE.game_id)
    return &TS2_NTSC_U_PROFILE;
  if (game_id == TS2_PAL_PROFILE.game_id)
    return &TS2_PAL_PROFILE;
  return nullptr;
}

static bool IsTS2()
{
  return GetTS2Profile() != nullptr;
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
  if (std::isfinite(camx) && std::isfinite(camy) && camx >= 0.f && camx < 360.f &&
      camy >= -90.f && camy <= 90.f)
  {
    return true;
  }

  const float alt_camx = ReadF32(system, address + TS2_ALT_CAM_X_OFFSET);
  const float alt_camy = ReadF32(system, address + TS2_ALT_CAM_Y_OFFSET);
  return std::isfinite(alt_camx) && std::isfinite(alt_camy) && alt_camx >= 0.f &&
         alt_camx < 360.f && alt_camy >= -90.f && alt_camy <= 90.f;
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
  const TS2MemoryProfile* const profile = GetTS2Profile();
  if (!profile)
    return 0;

  const u32 playerbase = system.GetMemory().Read_U32(profile->playerbase);
  const u32 player1base_coop = system.GetMemory().Read_U32(profile->player1base_coop);
  const u32 player2base_coop = system.GetMemory().Read_U32(profile->player2base_coop);

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
  const TS2MemoryProfile* const profile = GetTS2Profile();
  if (!profile)
    return TS2_DEFAULT_FOV;

  float fov = ReadF32(system, profile->fov);
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

static bool IsTS2AimMode(Core::System& system, u32 playerbase)
{
  return IsValidPlayerBase(playerbase) &&
         system.GetMemory().Read_U32(playerbase + TS2_AIM_MODE_OFFSET) == TS2_AIM_MODE_VALUE;
}

static float SmoothTowards(float current, float target, float step, float attack)
{
  const float change = std::clamp((target - current) * attack, -step, step);
  return current + change;
}

static float DecayAxis(float value)
{
  value *= s_camera_decay;
  return std::fabs(value) < 0.5f ? 0.0f : value;
}

static float ApplyAimResponseCurve(float position, float limit, float response_curve)
{
  if (position == 0.0f || limit <= 0.0f)
    return 0.0f;

  const float normalized = std::clamp(std::fabs(position) / limit, 0.0f, 1.0f);
  const float curved = std::pow(normalized, response_curve) * limit;
  return std::copysign(curved, position);
}

static void InjectTS2(Core::System& system, int ingame_pad, Delta delta)
{
  if (delta.dx == 0 && delta.dy == 0)
    return;

  const u32 playerbase = GetTS2PlayerBase(system, ingame_pad);
  if (!IsValidPlayerBase(playerbase))
    return;

  const TS2MemoryProfile* const profile = GetTS2Profile();
  if (!profile)
    return;

  if (IsTS2CameraMode(system, playerbase))
    return;

  const float fov = GetTS2FovForPlayer(system, playerbase);

  const float yaxislimit = ReadF32(system, profile->yaxislimit);
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
  if (!s_active)
    ClearCameraPadState();
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

void ToggleCameraMode()
{
  s_camera_mouse_mode = !s_camera_mouse_mode;
  ClearCameraPadState();
  Core::DisplayMessage(s_camera_mouse_mode ? "NetPlay 2P Mouse camera: mouse mode" :
                                             "NetPlay 2P Mouse camera: native mode",
                       2000);
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

void ApplyCameraModePadInput(int ingame_pad, const Delta& delta, GCPadStatus* pad_status)
{
  if (!pad_status || ingame_pad < 0 || ingame_pad >= static_cast<int>(s_camera_pad_x.size()))
    return;

  if (!s_camera_mouse_mode)
  {
    ClearCameraPadState(ingame_pad);
    return;
  }

  auto& system = Core::System::GetInstance();
  if (!Core::IsRunning(system) || !IsTS2())
    return;

  const u32 playerbase = GetTS2PlayerBase(system, ingame_pad);
  if (!IsTS2CameraMode(system, playerbase))
  {
    ClearCameraPadState(ingame_pad);
    return;
  }

  const bool restricted_aim = IsTS2AimMode(system, playerbase);
  if (restricted_aim != s_camera_restricted_aim[ingame_pad])
  {
    s_camera_raw_x[ingame_pad] = 0.0f;
    s_camera_raw_y[ingame_pad] = 0.0f;
    s_camera_pad_x[ingame_pad] = 0.0f;
    s_camera_pad_y[ingame_pad] = 0.0f;
    s_camera_aim_x[ingame_pad] = 0.0f;
    s_camera_aim_y[ingame_pad] = 0.0f;
    s_camera_restricted_aim[ingame_pad] = restricted_aim;
  }

  const float fov = GetTS2FovForPlayer(system, playerbase);
  float fov_factor = 1.0f;
  if (std::isfinite(fov) && fov > 3.0f)
    fov_factor = std::clamp(fov / TS2_DEFAULT_FOV, TS2_MIN_FOV_FACTOR, 1.0f);

  const auto to_axis = [](float offset, u8 center) {
    return static_cast<u8>(
        std::clamp(static_cast<int>(std::lround(static_cast<float>(center) + offset)), 0, 255));
  };

  if (restricted_aim)
  {
    s_camera_aim_x[ingame_pad] =
        std::clamp(s_camera_aim_x[ingame_pad] +
                       delta.dx * TS2_CAMERA_PAD_HORIZONTAL_DIRECTION *
                           s_camera_aim_horizontal_scale * fov_factor,
                   -s_camera_aim_horizontal_limit, s_camera_aim_horizontal_limit);
    s_camera_aim_y[ingame_pad] =
        std::clamp(s_camera_aim_y[ingame_pad] +
                       delta.dy * TS2_CAMERA_PAD_VERTICAL_DIRECTION *
                           s_camera_aim_vertical_scale * fov_factor,
                   -s_camera_aim_vertical_limit, s_camera_aim_vertical_limit);
    const float aim_output_x =
        ApplyAimResponseCurve(s_camera_aim_x[ingame_pad], s_camera_aim_horizontal_limit,
                              s_camera_aim_horizontal_response_curve);
    const float aim_output_y =
        ApplyAimResponseCurve(s_camera_aim_y[ingame_pad], s_camera_aim_vertical_limit,
                              s_camera_aim_vertical_response_curve);
    s_camera_pad_x[ingame_pad] = aim_output_x;
    s_camera_pad_y[ingame_pad] = aim_output_y;

    pad_status->substickX =
        to_axis(aim_output_x, GCPadStatus::C_STICK_CENTER_X);
    pad_status->substickY =
        to_axis(aim_output_y, GCPadStatus::C_STICK_CENTER_Y);

    return;
  }

  const float raw_target_x = delta.dx * TS2_CAMERA_PAD_HORIZONTAL_DIRECTION *
                             s_camera_horizontal_scale * fov_factor;
  s_camera_raw_x[ingame_pad] =
      std::clamp((s_camera_raw_x[ingame_pad] * s_camera_horizontal_accum_decay) +
                     raw_target_x,
                 -s_camera_horizontal_limit, s_camera_horizontal_limit);
  const float target_x =
      std::fabs(s_camera_raw_x[ingame_pad]) < 0.5f ? 0.0f : s_camera_raw_x[ingame_pad];
  const float raw_target_y = delta.dy * TS2_CAMERA_PAD_VERTICAL_DIRECTION *
                             s_camera_vertical_scale * fov_factor;
  s_camera_raw_y[ingame_pad] =
      std::clamp((s_camera_raw_y[ingame_pad] * s_camera_vertical_accum_decay) + raw_target_y,
                 -s_camera_vertical_limit, s_camera_vertical_limit);
  const float target_y =
      std::fabs(s_camera_raw_y[ingame_pad]) < 0.5f ? 0.0f : s_camera_raw_y[ingame_pad];

  s_camera_pad_x[ingame_pad] =
      target_x == 0.0f ? DecayAxis(s_camera_pad_x[ingame_pad]) :
                         SmoothTowards(s_camera_pad_x[ingame_pad], target_x,
                                       s_camera_horizontal_step, s_camera_horizontal_attack);
  s_camera_pad_y[ingame_pad] =
      target_y == 0.0f ? DecayAxis(s_camera_pad_y[ingame_pad]) :
                         SmoothTowards(s_camera_pad_y[ingame_pad], target_y,
                                       s_camera_vertical_step, s_camera_vertical_attack);

  pad_status->substickX = to_axis(s_camera_pad_x[ingame_pad], GCPadStatus::C_STICK_CENTER_X);
  pad_status->substickY = to_axis(s_camera_pad_y[ingame_pad], GCPadStatus::C_STICK_CENTER_Y);
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
