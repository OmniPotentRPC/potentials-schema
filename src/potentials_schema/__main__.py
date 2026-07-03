"""CLI for build-system discovery, in the numpy/metatensor helper style.

Examples::

    capnp compile -I"$(potentials-schema --include)" ...
    export PKG_CONFIG_PATH="$(potentials-schema --pkgconfig-path):$PKG_CONFIG_PATH"
    cmake -DCMAKE_PREFIX_PATH="$(potentials-schema --cmake-prefix-path)" ...
"""

import argparse

from . import (
    SCHEMA_ID,
    __version__,
    cmake_prefix_path,
    get_include,
    pkgconfig_path,
    schema_path,
)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(
        prog="potentials-schema",
        description="Print paths for consuming the packaged Potentials.capnp.",
    )
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument(
        "--schema-path", action="store_true", help="full path of Potentials.capnp"
    )
    group.add_argument(
        "--include",
        action="store_true",
        help="directory holding the schema (capnp -I import path)",
    )
    group.add_argument(
        "--pkgconfig-path",
        action="store_true",
        help="directory to append to PKG_CONFIG_PATH",
    )
    group.add_argument(
        "--cmake-prefix-path",
        action="store_true",
        help="prefix to append to CMAKE_PREFIX_PATH",
    )
    group.add_argument(
        "--schema-id", action="store_true", help="Cap'n Proto file id of the contract"
    )
    group.add_argument("--version", action="store_true", help="package version")
    args = parser.parse_args(argv)

    if args.schema_path:
        print(schema_path())
    elif args.include:
        print(get_include())
    elif args.pkgconfig_path:
        print(pkgconfig_path())
    elif args.cmake_prefix_path:
        print(cmake_prefix_path())
    elif args.schema_id:
        print(f"0x{SCHEMA_ID:x}")
    elif args.version:
        print(__version__)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
