#pragma once

#include "Common/CommonTypes.h"

struct GCPadStatus;

namespace MouseInjector
{
struct Delta
{
  float dx = 0;
  float dy = 0;
};

void Toggle();
bool IsActive();
void ToggleCameraMode();
void SetFocus(bool focused);
void DecreaseSensitivity();
void IncreaseSensitivity();
Delta CaptureDelta();
void ApplyCameraModePadInput(int ingame_pad, const Delta& delta, GCPadStatus* pad_status);
void ApplyNetPlayDelta(int ingame_pad, const Delta& delta);
void Update();
}  // namespace MouseInjector
