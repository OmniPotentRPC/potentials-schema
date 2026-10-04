# Changelog

This repository tags releases `vX.Y.Z`. GitHub release notes are generated from
the tag. This file records the schema changes in the unreleased version named
by `meson.build` and `pyproject.toml`.

## Unreleased (1.15.1)

- Append `UmaParams.intraopThreads` @7 and `interopThreads` @8 (`Int32`,
  default 1) and `deterministicAlgorithms` @9 (`Bool`, default false). An
  engine applies them once when it creates the model, so a host that does not
  link the tensor runtime still controls its threading and determinism.
- Append `MetatomicParams.intraopThreads` @7 and `interopThreads` @8 (same
  meaning), `nSymmetryRotations` @9 (`Int64`, 0), `randomRotation` @10,
  `so3ProbeScatter` @11 (`Bool`, false) and `torchDeterminism` @12
  (`MetatomicParams.TorchDeterminism`, `fast` or `deterministic`). They mirror
  rgpot's `MetatomicConfig`. `deterministic` is the deterministic-algorithms
  request for this arm, so it has no separate flag. The second value is not
  named `strict`: `STRICT` is a Windows SDK macro, and the generated C++ enum
  would not compile with `windows.h` included. No existing ordinal changes. The
  file id stays `@0xbd1f89fa17369103`.
- Append `Capabilities.buildVersion` @18 and `Capabilities.buildRevision` @19.
  Both are `Text` and empty when the producing build has no version or source
  revision. No existing ordinal changes. The file id stays
  `@0xbd1f89fa17369103`.
- Append `Potential.getCapabilities` @12 `() -> (capabilities :Capabilities)`
  so a client can read that message before `calculate`.
