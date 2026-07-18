#pragma once

#include "Common/CommonTypes.h"

namespace MouseInjector
{
struct Delta
{
  float dx = 0;
  float dy = 0;
};

void Toggle();
bool IsActive();
void SetFocus(bool focused);
void DecreaseSensitivity();
void IncreaseSensitivity();
Delta CaptureDelta();
void ApplyNetPlayDelta(int ingame_pad, const Delta& delta);
void Update();
}  // namespace MouseInjector
