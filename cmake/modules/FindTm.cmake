FIND_PATH( TM_INCLUDE_PATH vsx_tmi.h
  ~/vsxu-dev/tm/include
)

FIND_LIBRARY( TM_LIBRARY
  NAME tm64
  PATHS
  ~/vsxu-dev/tm/lib
)

FIND_LIBRARY( TM_REDIST_LIBRARY
  NAME tm64c
  PATHS
  ~/vsxu-dev/tm/redist
)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS("Tm Include Path" DEFAULT_MSG TM_INCLUDE_PATH)
FIND_PACKAGE_HANDLE_STANDARD_ARGS("Tm Static Library" DEFAULT_MSG TM_LIBRARY)
FIND_PACKAGE_HANDLE_STANDARD_ARGS("Tm Redist Library" DEFAULT_MSG TM_REDIST_LIBRARY)
