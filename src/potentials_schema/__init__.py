"""Canonical Cap'n Proto contract for OmniPotentRPC potential evaluations.

Ships ``Potentials.capnp`` (ForceInput, PotentialResult, PotentialConfig with
layered nwchem/cpmd backend arms) and helpers for pycapnp consumers.
"""

from importlib.resources import files
from pathlib import Path

__all__ = ["SCHEMA_ID", "__version__", "load", "schema_path"]

__version__ = "1.0.0"

SCHEMA_ID = 0xBD1F89FA17369103


def schema_path() -> Path:
    """Filesystem path of the packaged ``Potentials.capnp``."""
    return Path(str(files("potentials_schema").joinpath("Potentials.capnp")))


def load():
    """Parse the packaged schema with pycapnp and return the module.

    Requires the ``pycapnp`` extra: ``pip install potentials-schema[pycapnp]``.
    The returned module exposes ``ForceInput``, ``PotentialResult``,
    ``PotentialConfig``, ``NWChemParams``, ``CPMDParams``, and the rest of the
    schema as pycapnp types.
    """
    import capnp

    return capnp.load(str(schema_path()))
