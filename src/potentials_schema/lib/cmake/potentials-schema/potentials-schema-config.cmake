get_filename_component(_potentials_schema_pkg_dir
  "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

set(POTENTIALS_SCHEMA_DIR "${_potentials_schema_pkg_dir}")
set(POTENTIALS_SCHEMA_FILE "${_potentials_schema_pkg_dir}/Potentials.capnp")

if(NOT EXISTS "${POTENTIALS_SCHEMA_FILE}")
  set(potentials-schema_FOUND FALSE)
  set(potentials-schema_NOT_FOUND_MESSAGE
      "Potentials.capnp missing at ${POTENTIALS_SCHEMA_FILE}")
  return()
endif()

set(potentials-schema_FOUND TRUE)
unset(_potentials_schema_pkg_dir)
