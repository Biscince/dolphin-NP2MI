# Contributing to NP2MI

Thank you for considering a contribution to NP2MI.

NP2MI is an unofficial Dolphin fork focused on synchronized mouse input for
multiplayer games over NetPlay. The currently supported profile is
TimeSplitters 2 NTSC-U (`GTSE4F`).

## Project Scope

Contributions are welcome for:

- fixes to synchronized mouse input and player assignment;
- improvements to supported camera, aiming, and turret modes;
- new regional or game input profiles;
- NetPlay compatibility and determinism fixes related to NP2MI;
- documentation, packaging, and reproducible build improvements.

Changes that improve Dolphin generally, without being specific to NP2MI, are
usually better submitted to the
[upstream Dolphin project](https://github.com/dolphin-emu/dolphin). Upstream
contributions must follow Dolphin's own contribution and legal policies.

## Before Opening a Pull Request

- Search existing issues and pull requests for related work.
- Open an issue before starting a large feature or a new game profile.
- Keep changes focused and avoid unrelated Dolphin refactoring.
- Target the `np2mi` branch. The `master` branch is kept as an upstream
  reference.
- Do not commit ROMs, disc images, save data, memory dumps, copyrighted game
  assets, build output, or personal Dolphin configuration files.

## Building on Windows

Initialize the submodules:

```sh
git submodule update --init --recursive
```

Open `Source/dolphin-emu.sln` in Visual Studio, select `Release` and `x64`, and
build the `Dolphin` project. Use the toolset configured by the repository; do
not bulk-retarget every project unless a toolchain migration is intentional.

## Testing Expectations

Every code change should compile successfully in its affected configuration.
Changes to mouse injection, memory profiles, or NetPlay should also be tested
with the relevant game.

For TimeSplitters 2 changes, test as many of these cases as applicable:

- player 1 and player 2 on separate NetPlay instances;
- normal horizontal and vertical aiming;
- sniper aiming at minimum and maximum zoom;
- camera and turret entry, aiming, and exit;
- mouse/native camera mode switching;
- sensitivity hotkeys;
- multiple story levels;
- a session using the recommended `No Save Data` configuration.

Describe the performed tests and any remaining limitations in the pull
request. Memory changes should include the game ID, region, level or mode, and
the method used to validate the address or pointer.

## Code Style

NP2MI follows Dolphin's established C++ style. Refer to
[Dolphin's contribution guide](https://github.com/dolphin-emu/dolphin/blob/master/Contributing.md)
for detailed formatting and naming conventions.

Run the relevant formatter and check for whitespace errors before submitting:

```sh
git diff --check
```

## Reverse Engineering and Legal Requirements

Only use information obtained legally through public documentation,
independent runtime observation, debugging, and testing with legitimately
obtained software.

Do not use or submit:

- confidential information covered by an NDA;
- leaked Nintendo, console, game, or SDK material;
- proprietary source code or symbols;
- pirated game data;
- code or documentation derived from any of the above.

Addresses, offsets, and original analysis may be documented when they can be
reproduced without distributing copyrighted game content.

## AI-Assisted Contributions

AI-assisted contributions are allowed, but contributions must come from a
human who understands and takes responsibility for the submitted work.

When AI assistance is substantial, the contributor must:

- disclose the tool and the parts of the contribution it assisted with;
- review and understand every submitted change;
- test the resulting behavior;
- verify technical claims through source code, documentation, or reproducible
  observation;
- ensure that no confidential, leaked, proprietary, or incompatible licensed
  material was used;
- be able to explain and maintain the implementation.

Unreviewed, unexplained, or unverifiable generated contributions may be
rejected. AI tools must not be used as a source of claims about proprietary
console or game internals.

A short disclosure in the pull request is sufficient, for example:

```text
AI assistance: OpenAI Codex helped draft the implementation and tests.
Human verification: The changes were reviewed, built, and tested in a
two-player NetPlay session by the contributor.
```

NP2MI itself has been developed with AI-assisted coding using OpenAI Codex.
Integrated changes are reviewed and tested by the project maintainer.

## Licensing

By submitting a contribution, you agree that it will be licensed under the
GNU General Public License version 2 or any later version, consistent with the
rest of the Dolphin codebase.

## Questions and Bug Reports

Use this repository's GitHub issues for reproducible bugs, compatibility
reports, and development questions. Do not request or share copyrighted game
files.
