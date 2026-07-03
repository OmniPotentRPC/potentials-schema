"""Canonical Cap'n Proto contract for OmniPotentRPC potential evaluations.

Ships ``Potentials.capnp`` (ForceInput, PotentialResult, PotentialConfig with
layered nwchem/cpmd backend arms) plus build-system helpers in the style of
``numpy.get_include()`` / metatensor's cmake helpers:

- :func:`schema_path` / :func:`get_include` for capnp import paths,
- :func:`pkgconfig_path` for ``PKG_CONFIG_PATH`` (data-only ``.pc`` exposing
  ``schemafile`` / ``schemadir`` variables),
- :func:`cmake_prefix_path` for ``CMAKE_PREFIX_PATH`` so
  ``find_package(potentials-schema CONFIG)`` defines
  ``POTENTIALS_SCHEMA_FILE`` / ``POTENTIALS_SCHEMA_DIR``,
- :func:`load` for pycapnp users.

The same paths are printable from a shell via ``potentials-schema --help``
(or ``python -m potentials_schema``) for meson ``run_command()`` wiring.
"""

from importlib.resources import files
from pathlib import Path

__all__ = [
    "SCHEMA_ID",
    "__version__",
    "cmake_prefix_path",
    "get_include",
    "load",
    "pkgconfig_path",
    "schema_path",
]

__version__ = "1.3.0"

SCHEMA_ID = 0xBD1F89FA17369103


def _package_dir() -> Path:
    return Path(str(files("potentials_schema").joinpath("Potentials.capnp"))).parent


def schema_path() -> Path:
    """Filesystem path of the packaged ``Potentials.capnp``."""
    return _package_dir() / "Potentials.capnp"


def get_include() -> str:
    """Directory holding ``Potentials.capnp`` for capnp ``-I`` import paths."""
    return str(_package_dir())


def pkgconfig_path() -> str:
    """Directory to append to ``PKG_CONFIG_PATH``.

    Exposes the data-only ``potentials-schema`` module whose ``schemafile``
    and ``schemadir`` variables locate the packaged contract::

        pkg-config --variable=schemafile potentials-schema
    """
    return str(_package_dir() / "lib" / "pkgconfig")


def cmake_prefix_path() -> str:
    """Prefix to append to ``CMAKE_PREFIX_PATH``.

    Makes ``find_package(potentials-schema CONFIG REQUIRED)`` resolve and set
    ``POTENTIALS_SCHEMA_FILE`` / ``POTENTIALS_SCHEMA_DIR``.
    """
    return str(_package_dir())


def load():
    """Parse the packaged schema with pycapnp and return the module.

    Requires the ``pycapnp`` extra: ``pip install potentials-schema[pycapnp]``.
    The returned module exposes ``ForceInput``, ``PotentialResult``,
    ``PotentialConfig``, ``NWChemParams``, ``CPMDParams``, and the rest of the
    schema as pycapnp types.
    """
    import capnp

    return capnp.load(str(schema_path()))
