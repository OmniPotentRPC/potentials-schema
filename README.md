# potentials-schema

Canonical Cap'n Proto contract for OmniPotentRPC potential evaluations.
`Potentials.capnp` is the single source of truth consumed by
[rgpot](../rgpot) (the eOn-facing touchpoint), [nwchemc](../nwchemc), and
[cpmdc](../cpmdc). Per-repo copies of this file are drift bugs.

## Layout

One file, two tiers:

- **Shared carriers** (code-agnostic physics): `ForceInput` for per-step
  geometry/cell/units, `PotentialResult` for energies, forces, Hessians,
  stress, properties, frequencies, and thermochemistry, plus the `Potential`
  RPC interface.
- **Backend arms** (code-native method config): `NWChemParams` and
  `CPMDParams` inside the `PotentialConfig` union. Each arm mirrors its own
  code's input language (NWChem stanzas and RTDB sets, CPMD `&SECTION`
  decks). Arms are layered side by side and never normalized into a common
  config vocabulary; the long tail of code-specific knobs stays code-specific
  through each arm's generic escape hatches (`NWChemGenericStanza`,
  `CPMDSetDirective`, `CPMDInputSection.raw`).

`CommonMethodSpec` (`PotentialConfig.common`) is the thin normalized overlay:
NOMAD-metainfo-aligned quantities (libxc functional names, Monkhorst-Pack
k mesh, smearing, SCF thresholds) that lower one-way into the native arms.
Backends apply it first; any knob the native arm also sets wins. It never
replaces the arms.

## Evolution rules

- Field ordinals are append-only. Never renumber, retype, or reuse an
  ordinal; deprecate by comment.
- New backends claim the next free `PotentialConfig` union ordinal
  (`metatomic @3`, `xtb @4`, `tblite @5` are reserved in comments).
- The file ID `@0xbd1f89fa17369103` never changes.
- Every change lands here first; consumers vendor or pin this repo and their
  CI must fail when their copy diverges.

## Validation

```sh
capnp compile -o- Potentials.capnp > /dev/null
```

## Consuming

- **Python (pycapnp)**: `pip install potentials-schema[pycapnp]`, then
  `potentials_schema.load()` or `potentials_schema.schema_path()`.
- **From an installed wheel (numpy/metatensor style)**: the package ships a
  data-only pkg-config module and a CMake config next to the schema.
  `potentials-schema --include | --schema-path | --pkgconfig-path |
  --cmake-prefix-path | --schema-id` prints the paths for shell/meson wiring,
  and `potentials_schema.get_include()/pkgconfig_path()/cmake_prefix_path()`
  do the same from Python. With `PKG_CONFIG_PATH` extended,
  `pkg-config --variable=schemafile potentials-schema` locates the contract;
  with `CMAKE_PREFIX_PATH` extended, `find_package(potentials-schema CONFIG)`
  sets `POTENTIALS_SCHEMA_FILE`/`POTENTIALS_SCHEMA_DIR`.
- **meson**: wrap-file pinned to a release tarball; the subproject exposes
  `Potentials.capnp` at its root. From a wheel:
  `run_command('potentials-schema', '--schema-path')`.
- **CMake**: `FetchContent_Declare` on the release tarball URL, or the wheel's
  `find_package` config above.

Releases are tagged `vX.Y.Z`; wheels publish to PyPI via OIDC trusted
publishing and artifacts attach to the GitHub release.
