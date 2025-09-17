# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Types of changes: Added, Changed, Deprecated, Removed, Fixed, Security

## [Unreleased]

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
