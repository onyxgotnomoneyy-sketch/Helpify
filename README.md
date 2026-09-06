# Helpify — Geode 5.10.1 source project

This project targets **Geometry Dash 2.2081 + Geode v5.10.1**, including iOS.

## Features

- Floating Helpify icon using the supplied hand-drawn logo
- Generates a level name and level concept
- Rename the assistant from Geode settings
- Resize the floating icon
- Move the icon with X/Y settings
- Optional TTS setting is included, but the current build intentionally does not call an iOS speech API yet

## iOS build

This is a **source project**, not a `.geode` package. iOS compilation requires macOS with Xcode/iPhone SDK and the iOS Geode binaries installed.

```sh
geode sdk update v5.10.1
geode sdk install-binaries --platform ios --version v5.10.1
geode build -p ios
```

The resulting `.geode` will be produced by the Geode build system.

The GitHub Actions workflow can also build the iOS target on a macOS runner.
