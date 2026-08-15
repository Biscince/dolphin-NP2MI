# Dolphin NP2MI

NP2MI is a custom Dolphin build for synchronized 2-player mouse aiming over
NetPlay. Mouse movement is sent with Dolphin's NetPlay input data and applied
only to the assigned in-game player.

## Download

Download the latest portable build from the
[GitHub Releases page](https://github.com/Biscince/dolphin-NP2MI/releases).
Extract the archive to a folder and launch `Dolphin.exe`; installation is not
required.

If the Releases page does not contain a downloadable asset, no public binary
has been published yet. NP2MI does not include any game files.

## Version

```text
NP2MI v0.1.0
Dolphin base: 2606-143
```

All NetPlay players must use the exact same NP2MI build and game version.

## Supported Titles

| Game Title | Version | Input Profile | Mouse Support | Issues |
| --- | :---: | :---: | :---: | ----------- |
| TimeSplitters 2 | NTSC-U (`GTSE4F`) | :heavy_check_mark: | Fair | <sup>Turret/camera vertical input can feel inconsistent in some levels.</sup> |
| TimeSplitters 2 | PAL (`GTSP4F`) | :x: | None | <sup>Memory profile not implemented yet.</sup> |
| TimeSplitters: Future Perfect | Any | :x: | None | <sup>No NP2MI input profile is currently implemented.</sup> |

The complete TimeSplitters 2 NTSC-U 2-player story campaign has been tested on
two PCs over NetPlay.

## Intended Use

The recommended setup is two PCs, one Dolphin instance and one mouse player per
PC. NP2MI is not currently intended as a polished local split-screen mouse
solution on a single PC.

Solo play is also supported, but the game must still be launched through a
NetPlay session.

## Main Features

- NetPlay-synchronized mouse aiming.
- Player 1/player 2 targeting for TimeSplitters 2 co-op.
- Per-player FOV handling for sniper zoom.
- Mouse control in supported camera and turret sequences.
- Restricted aiming behavior for scoped and turret views.
- Mouse/native camera mode toggle.
- Adjustable sensitivity.
- Minimal on-screen messages.

## Included Controller Profiles

The portable release includes keyboard and mouse profiles from the original
injector:

- `TimeSplitters` (recommended for TimeSplitters 2)

Open Dolphin's controller settings, configure the emulated GameCube controller
for your assigned port, select the appropriate profile, and load it.

## Default Hotkeys

The portable release includes these default bindings:

```text
4                     Toggle NetPlay 2P Mouse
5                     Toggle mouse/native camera mode
Numpad - (SUBTRACT)   Decrease NetPlay 2P Mouse Sensitivity
Numpad + (ADD)        Increase NetPlay 2P Mouse Sensitivity
```

Change them under **Options > Hotkey Settings** if they conflict with your
keyboard layout or existing Dolphin configuration.

## Starting a NetPlay Session

1. Make sure both players use the exact same NP2MI build, TimeSplitters 2
   NTSC-U game image, cheats, patches, and game-changing settings.
2. In Dolphin, open **Tools > Start NetPlay**.
3. One player hosts the session and selects TimeSplitters 2; the other player
   joins that session.
4. Assign PC 1 to GameCube Port 1 and PC 2 to GameCube Port 2.
5. In the NetPlay window, open the **Data** menu and select **No Save Data**.
6. Start the game, then press `4` locally on each PC to enable mouse injection.

Recommended assignment:

```text
PC 1 -> GameCube Port 1 -> Player 1
PC 2 -> GameCube Port 2 -> Player 2
```

`No Save Data` is recommended because it prevented the campaign desyncs seen
during testing. Both players should also avoid mismatched memory cards.

Port forwarding is not covered here because the required setup depends on the
router and connection method. Existing Dolphin NetPlay guides can be used when
manual port configuration is necessary.

## In-Game Settings

After starting TimeSplitters 2:

1. Open the game's control options and disable **Auto Lookahead**. It interferes
   with mouse aiming.
2. If desired, change the controller configuration from **Classic** to
   **Custom**. This allows crouch and secondary fire to be swapped, along with
   any other preferred button changes.

## Known Limitations

- Only TimeSplitters 2 NTSC-U is currently supported.
- Vertical mouse movement in turret sequences (Siberia, Return to Planet X, and
  Robot Factory) is less consistent than normal first-person aiming.
- Camera and turret control emulates native C-stick input and can retain some
  of the original game's aiming behavior.
- The implementation still uses hardcoded TimeSplitters 2 memory offsets.
- Local split-screen mouse control on a single PC is not the intended setup.
- NetPlay desyncs may still happen because of save data or mismatched settings.

## Roadmap (No Commitments)

Possible future work, in the current order of interest:

1. TimeSplitters: Future Perfect mouse support.
2. Mouse support for up to four players in Arcade mode.
3. Additional game and region profiles, including TimeSplitters 2 PAL.
4. A dedicated NP2MI settings page in Dolphin.

## Community

- [TimeSplitters Online](https://discord.com/servers/timesplitters-online-467580641639268362)
  organizes online matches and provides setup help.
- The [TimeSplitters: Rewind community](https://www.timesplittersrewind.com/)
  maintains its Discord link on the official project website.

These communities are independent from NP2MI.

## Original Dolphin README for anyone who wants to build it

# Dolphin - A GameCube and Wii Emulator

[Homepage](https://dolphin-emu.org/) | [Project Site](https://github.com/dolphin-emu/dolphin) | [Buildbot](https://dolphin.ci/) | [Forums](https://forums.dolphin-emu.org/) | [Wiki](https://wiki.dolphin-emu.org/) | [GitHub Wiki](https://github.com/dolphin-emu/dolphin/wiki) | [Issue Tracker](https://bugs.dolphin-emu.org/projects/emulator/issues) | [Coding Style](https://github.com/dolphin-emu/dolphin/blob/master/Contributing.md) | [Transifex Page](https://app.transifex.com/dolphinemu/dolphin-emu/dashboard/) | [Analytics](https://mon.dolphin-emu.org/)

Dolphin is an emulator for running GameCube and Wii games on Windows,
Linux, macOS, and recent Android devices. It's licensed under the terms
of the GNU General Public License, version 2 or later (GPLv2+).

Please read the [FAQ](https://dolphin-emu.org/docs/faq/) before using Dolphin.

## System Requirements

### Desktop

* OS
    * Windows (10 1903 or higher).
    * Linux.
    * macOS (11.0 Big Sur or higher).
    * Unix-like systems other than Linux are not officially supported but might work.
* Processor
    * A CPU with SSE2 support.
    * A modern CPU (3 GHz and Dual Core, not older than 2008) is highly recommended.
* Graphics
    * A reasonably modern graphics card (Direct3D 11.1 / OpenGL 3.3).
    * A graphics card that supports Direct3D 11.1 / OpenGL 4.4 is recommended.

### Android

* OS
    * Android (7.0 Nougat or higher).
* Processor
    * A processor with support for 64-bit applications (either ARMv8 or x86-64).
* Graphics
    * A graphics processor that supports OpenGL ES 3.0 or higher. Performance varies heavily with [driver quality](https://dolphin-emu.org/blog/2013/09/26/dolphin-emulator-and-opengl-drivers-hall-fameshame/).
    * A graphics processor that supports standard desktop OpenGL features is recommended for best performance.

Dolphin can only be installed on devices that satisfy the above requirements. Attempting to install on an unsupported device will fail and display an error message.

## Building for Windows

Use the solution file `Source/dolphin-emu.sln` to build Dolphin on Windows.
Dolphin targets the latest MSVC shipped with Visual Studio or Build Tools.
Other compilers might be able to build Dolphin on Windows but have not been
tested and are not recommended to be used. Git and latest Windows SDK must be
installed when building.

Make sure to pull submodules before building:
```sh
git submodule update --init --recursive
```

The "Release" solution configuration includes performance optimizations for the best user experience but complicates debugging Dolphin.
The "Debug" solution configuration is significantly slower, more verbose and less permissive but makes debugging Dolphin easier.

## Building for Linux and macOS

Dolphin requires [CMake](https://cmake.org/) for systems other than Windows.
You need a recent version of GCC or Clang with decent c++20 support. CMake will
inform you if your compiler is too old.
Many libraries are bundled with Dolphin and used if they're not installed on
your system. CMake will inform you if a bundled library is used or if you need
to install any missing packages yourself. You may refer to the [wiki](https://github.com/dolphin-emu/dolphin/wiki/Building-for-Linux) for more information.

Make sure to pull submodules before building:
```sh
git submodule update --init --recursive
```

### macOS Build Steps:

A binary supporting a single architecture can be built using the following steps:

1. `mkdir build`
2. `cd build`
3. `cmake ..`
4. `make -j $(sysctl -n hw.logicalcpu)`

An application bundle will be created in `./Binaries`.

A script is also provided to build universal binaries supporting both x64 and ARM in the same
application bundle using the following steps:

1. `mkdir build`
2. `cd build`
3. `python ../BuildMacOSUniversalBinary.py`
4. Universal binaries will be available in the `universal` folder

Doing this is more complex as it requires installation of library dependencies for both x64 and ARM (or universal library
equivalents) and may require specifying additional arguments to point to relevant library locations.
Execute BuildMacOSUniversalBinary.py --help for more details.

### Linux Global Build Steps:

To install to your system.

1. `mkdir build`
2. `cd build`
3. `cmake ..`
4. `make -j $(nproc)`
5. `sudo make install`

### Linux Local Build Steps:

Useful for development as root access is not required.

1. `mkdir Build`
2. `cd Build`
3. `cmake .. -DLINUX_LOCAL_DEV=true`
4. `make -j $(nproc)`
5. `ln -s ../../Data/Sys Binaries/`

### Linux Portable Build Steps:

Can be stored on external storage and used on different Linux systems.
Or useful for having multiple distinct Dolphin setups for testing/development/TAS.

1. `mkdir Build`
2. `cd Build`
3. `cmake .. -DLINUX_LOCAL_DEV=true`
4. `make -j $(nproc)`
5. `cp -r ../Data/Sys/ Binaries/`
6. `touch Binaries/portable.txt`

## Building for Android

These instructions assume familiarity with Android development. If you do not have an
Android dev environment set up, see [AndroidSetup.md](AndroidSetup.md).

Make sure to pull submodules before building:
```sh
git submodule update --init --recursive
```

If using Android Studio, import the Gradle project located in `./Source/Android`.

Android apps are compiled using a build system called Gradle. Dolphin's native component,
however, is compiled using CMake. The Gradle script will attempt to run a CMake build
automatically while building the Java code.

## Uninstalling

On Windows, simply remove the extracted directory, unless it was installed with the NSIS installer,
in which case you can uninstall Dolphin like any other Windows application.

Linux users can run `cat install_manifest.txt | xargs -d '\n' rm` as root from the build directory
to uninstall Dolphin from their system.

macOS users can simply delete Dolphin.app to uninstall it.

Additionally, you'll want to remove the global user directory if you don't plan on reinstalling Dolphin.

## Command Line Usage

```
Usage: Dolphin.exe [options]... [FILE]...

Options:
  --version             show program's version number and exit
  -h, --help            show this help message and exit
  -u USER, --user=USER  User folder path
  -m MOVIE, --movie=MOVIE
                        Play a movie file
  -e <file>, --exec=<file>
                        Load the specified file
  -n <16-character ASCII title ID>, --nand_title=<16-character ASCII title ID>
                        Launch a NAND title
  -C <System>.<Section>.<Key>=<Value>, --config=<System>.<Section>.<Key>=<Value>
                        Set a configuration option
  -s <file>, --save_state=<file>
                        Load the initial save state
  -d, --debugger        Show the debugger pane and additional View menu options
  -l, --logger          Open the logger
  -b, --batch           Run Dolphin without the user interface (Requires
                        --exec or --nand-title)
  -c, --confirm         Set Confirm on Stop
  -v VIDEO_BACKEND, --video_backend=VIDEO_BACKEND
                        Specify a video backend
  -a AUDIO_EMULATION, --audio_emulation=AUDIO_EMULATION
                        Choose audio emulation from [HLE|LLE]
```

Available DSP emulation engines are HLE (High Level Emulation) and
LLE (Low Level Emulation). HLE is faster but less accurate whereas
LLE is slower but close to perfect. Note that LLE has two submodes (Interpreter and Recompiler)
but they cannot be selected from the command line.

Available video backends are "D3D" and "D3D12" (they are only available on Windows), "OGL", and "Vulkan".
There's also "Null", which will not render anything, and
"Software Renderer", which uses the CPU for rendering and
is intended for debugging purposes only.

## DolphinTool Usage
```
usage: dolphin-tool COMMAND -h

commands supported: [convert, verify, header, extract]
```

```
Usage: convert [options]... [FILE]...

Options:
  -h, --help            show this help message and exit
  -u USER, --user=USER  User folder path, required for temporary processing
                        files.Will be automatically created if this option is
                        not set.
  -i FILE, --input=FILE
                        Path to disc image FILE.
  -o FILE, --output=FILE
                        Path to the destination FILE.
  -f FORMAT, --format=FORMAT
                        Container format to use. Default is RVZ. [iso|gcz|wia|rvz]
  -s, --scrub           Scrub junk data as part of conversion.
  -b BLOCK_SIZE, --block_size=BLOCK_SIZE
                        Block size for GCZ/WIA/RVZ formats, as an integer.
                        Suggested value for RVZ: 131072 (128 KiB)
  -c COMPRESSION, --compression=COMPRESSION
                        Compression method to use when converting to WIA/RVZ.
                        Suggested value for RVZ: zstd [none|zstd|bzip|lzma|lzma2]
  -l COMPRESSION_LEVEL, --compression_level=COMPRESSION_LEVEL
                        Level of compression for the selected method. Ignored
                        if 'none'. Suggested value for zstd: 5
```

```
Usage: verify [options]...

Options:
  -h, --help            show this help message and exit
  -u USER, --user=USER  User folder path, required for temporary processing
                        files.Will be automatically created if this option is
                        not set.
  -i FILE, --input=FILE
                        Path to disc image FILE.
  -a ALGORITHM, --algorithm=ALGORITHM
                        Optional. Compute and print the digest using the
                        selected algorithm, then exit. [crc32|md5|sha1|rchash]
```

```
Usage: header [options]...

Options:
  -h, --help            show this help message and exit
  -i FILE, --input=FILE
                        Path to disc image FILE.
  -b, --block_size      Optional. Print the block size of GCZ/WIA/RVZ formats,
then exit.
  -c, --compression     Optional. Print the compression method of GCZ/WIA/RVZ
                        formats, then exit.
  -l, --compression_level
                        Optional. Print the level of compression for WIA/RVZ
                        formats, then exit.
```

```
Usage: extract [options]...

Options:
  -h, --help            show this help message and exit
  -i FILE, --input=FILE
                        Path to disc image FILE.
  -o FOLDER, --output=FOLDER
                        Path to the destination FOLDER.
  -p PARTITION, --partition=PARTITION
                        Which specific partition you want to extract.
  -s SINGLE, --single=SINGLE
                        Which specific file/directory you want to extract.
  -l, --list            List all files in volume/partition. Will print the
                        directory/file specified with --single if defined.
  -q, --quiet           Mute all messages except for errors.
  -g, --gameonly        Only extracts the DATA partition.
```
