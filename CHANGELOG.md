# Changelog

This repository tags releases `vX.Y.Z`. GitHub release notes are generated from
the tag. This file records the schema changes in the unreleased version named
by `meson.build` and `pyproject.toml`.

## Unreleased (1.15.1)

- Append `Capabilities.buildVersion` @18 and `Capabilities.buildRevision` @19.
  Both are `Text` and empty when the producing build has no version or source
  revision. No existing ordinal changes. The file id stays
  `@0xbd1f89fa17369103`.
- Append `Potential.getCapabilities` @12 `() -> (capabilities :Capabilities)`
  so a client can read that message before `calculate`.
