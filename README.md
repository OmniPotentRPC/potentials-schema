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

A thin normalized convenience struct (xc functional, basis, SCF tolerances)
that lowers one-way into the native arms can be added later as a new field;
it must not replace the arms.

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
- **meson**: wrap-file pinned to a release tarball; the subproject exposes
  `Potentials.capnp` at its root.
- **CMake**: `FetchContent_Declare` on the release tarball URL.

Releases are tagged `vX.Y.Z`; wheels publish to PyPI via OIDC trusted
publishing and artifacts attach to the GitHub release.
