# Minimum potential ABI profile

A backend shim (nwchemc, cpmdc, future lammpsc/gromacsc/...) is
plug-compatible with rgpot's loader when it exports the following C symbols,
prefix-parameterized on `<p>` (`nwchemc`, `cpmdc`, ...). Everything is fully
native: flat binary Cap'n Proto messages from this repo's `Potentials.capnp`
in both directions, plain C calls in-process. No JSON, no string-command
parsing, no side-channel config files.

## Lifecycle and diagnostics

| Symbol | Contract |
| --- | --- |
| `int <p>_abi_version(void)` | Numeric ABI generation; matches the header's `<P>_ABI_VERSION` and the shared-library soversion. |
| `const char *<p>_version(void)` | Human-readable `"<p>/<semver>"`. |
| `int <p>_available(void)` | 1 when the embedded engine is linked and usable. |
| `const char *<p>_last_error(void)` | Thread-local diagnostic for the most recent int-returning configuration call; empty string on success. |
| `void <p>_finalize(void)` | Releases an owned engine runtime. |

## Configuration (PotentialConfig bytes)

| Symbol | Contract |
| --- | --- |
| `int <p>_configure(const void *config, size_t size)` | Applies a `PotentialConfig`: the backend's union arm wins where it differs from schema defaults; a set `common` overlay (`CommonMethodSpec`) lowers into the native configuration and fills the rest. Overlay fields without a faithful lowering fail with a `last_error` explanation. |
| `<P>Session *<p>_session_create_from_config(const void *config, size_t size)` | Same resolution onto a fresh persistent session. |
| `int <p>_session_configure(<P>Session *, const void *config, size_t size)` | Reconfigure before the session accepts a topology. |
| `void <p>_session_destroy(<P>Session *)` | Release the session. |

## Evaluation (ForceInput bytes -> PotentialResult bytes)

| Symbol | Contract |
| --- | --- |
| `size_t <p>_potential_result_size_for_force_input(const void *fi, size_t size)` | Buffer size the result carrier needs for this step. |
| `<P>Result <p>_session_calculate_result(<P>Session *, const void *fi, size_t fi_size, void *out, size_t capacity, size_t *written)` | One step on a persistent session; energies/forces/properties come back as a `PotentialResult` message. |
| `<P>Result <p>_calculate_result_from_config(const void *config, size_t config_size, const void *fi, size_t fi_size, void *out, size_t capacity, size_t *written)` | One-shot config + step. |

`<P>Result` is the backend's POD `{int ok; double energy_h; char message[512];}`.

## Conformance

- nwchemc: exports the full profile.
- cpmdc: exports the full profile except `cpmdc_calculate_result_from_config`
  (tracked); `cpmdc_calculate_result` covers the one-shot params path.
- Each backend's dlopen test must load every profile symbol from the built
  shared library.

## Capability discovery (planned)

`int <p>_capabilities_result(void *out, size_t capacity, size_t *written)`
returning a capnp `Capabilities` message (supported operations, lowered
`CommonMethodSpec` fields, engine name/version) so drivers negotiate before
dispatch. Tracked alongside the schema struct addition.
