// Copyright 2008 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Common/Version.h"

#include <string>

#include "Common/scmrev.h"

namespace Common
{
#define EMULATOR_NAME "Dolphin"

#ifdef _DEBUG
#define BUILD_TYPE_STR "Debug "
#elif defined DEBUGFAST
#define BUILD_TYPE_STR "DebugFast "
#else
#define BUILD_TYPE_STR ""
#endif

#define DOLPHIN_BASE_VERSION "2606-143"
#define NETPLAY_2P_MOUSE_VERSION "-NP2MI-0.1.0"

const std::string& GetEmulatorName()
{
  static const std::string emulator_name = EMULATOR_NAME;
  return emulator_name;
}

const std::string& GetScmRevStr()
{
  static const std::string scm_rev_str = EMULATOR_NAME " "
  // Note this macro can be empty if the master branch does not exist.
#if 1 - SCM_COMMITS_AHEAD_MASTER - 1 != 0
                                                       "[" SCM_BRANCH_STR "] "
#endif

#ifdef __INTEL_COMPILER
      BUILD_TYPE_STR DOLPHIN_BASE_VERSION NETPLAY_2P_MOUSE_VERSION "-ICC";
#else
      BUILD_TYPE_STR DOLPHIN_BASE_VERSION NETPLAY_2P_MOUSE_VERSION;
#endif
  return scm_rev_str;
}

const std::string& GetScmRevGitStr()
{
  static const std::string scm_rev_git_str = SCM_REV_STR NETPLAY_2P_MOUSE_VERSION;
  return scm_rev_git_str;
}

const std::string& GetScmDescStr()
{
  static const std::string scm_desc_str = DOLPHIN_BASE_VERSION NETPLAY_2P_MOUSE_VERSION;
  return scm_desc_str;
}

const std::string& GetScmBranchStr()
{
  static const std::string scm_branch_str = SCM_BRANCH_STR;
  return scm_branch_str;
}

const std::string& GetUserAgentStr()
{
  static const std::string user_agent_str =
      EMULATOR_NAME "/" DOLPHIN_BASE_VERSION NETPLAY_2P_MOUSE_VERSION;
  return user_agent_str;
}

const std::string& GetScmDistributorStr()
{
  static const std::string scm_distributor_str = SCM_DISTRIBUTOR_STR;
  return scm_distributor_str;
}

const std::string& GetScmUpdateTrackStr()
{
  static const std::string scm_update_track_str = SCM_UPDATE_TRACK_STR;
  return scm_update_track_str;
}

const std::string& GetNetplayDolphinVer()
{
#ifdef _WIN32
  static const std::string netplay_dolphin_ver =
      DOLPHIN_BASE_VERSION NETPLAY_2P_MOUSE_VERSION " Win";
#elif __APPLE__
  static const std::string netplay_dolphin_ver =
      DOLPHIN_BASE_VERSION NETPLAY_2P_MOUSE_VERSION " Mac";
#else
  static const std::string netplay_dolphin_ver =
      DOLPHIN_BASE_VERSION NETPLAY_2P_MOUSE_VERSION " Lin";
#endif
  return netplay_dolphin_ver;
}

int GetScmCommitsAheadMaster()
{
  // Note this macro can be empty if the master branch does not exist.
  return SCM_COMMITS_AHEAD_MASTER + 0;
}

}  // namespace Common
