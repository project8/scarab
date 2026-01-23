# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Types of changes: Added, Changed, Deprecated, Removed, Fixed, Security

## [Unreleased]

## [3.14.1] - 2026-01-23

### Added

- Python cancelation tests

### Fixed

- Give hint to find spdlog during build in the cmake config file
- Set CMake policy 167 to NEW


## [3.14.0] - 2026-01-19

### Changed

- NOTE: signal_handler's constructor now requires bool argument to flag whether to use its own signal-watching thread
- Signal handling now requires a dedicated thread
- Updated GHA runners
- Removed local loggers

### Fixed

- Tied spdlog to a tag and not a branch
- Updated spdlog fork from upstream
- signal_handler signal handling functions use only approved function calls
- Check if spdlog thread pool exists before making a new one


## [3.13.4] - 2025-11-04

### Fixed

- Find spdlog dependency in ScarabConfig.cmake.in
- Set spdlog_DIR in CMakeLists.txt if Scarab has a parent directory


## [3.13.3] - 2025-10-06

### Changed

- Default threshold configurable with preprocessor macro and via CMake
- Splash screen callback added to main_app


## [3.13.2] - 2025-09-17

### Changed

- Updated CLI11 to v2.5.0

### Fixed

- Issue with CLI11 flags fixed

### Removed

- Removed the logging test directory


## [3.13.1] - 2025-09-04

### Added

- Minimum version for GCC is now enforced in PackageBuilder.cmake; minimum version is 12
- Added a changelog and GHA job to create a GH Release with the changelog information

### Fixed

- Use GCC 12 on Ubuntu 22.04 GHA runners


## [3.13.0] -- 2025-08-26

### Changed

- Logger now uses spdlog as its backend
