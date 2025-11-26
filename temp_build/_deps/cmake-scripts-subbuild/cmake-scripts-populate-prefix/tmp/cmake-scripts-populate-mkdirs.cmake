# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-src")
  file(MAKE_DIRECTORY "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-src")
endif()
file(MAKE_DIRECTORY
  "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-build"
  "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-subbuild/cmake-scripts-populate-prefix"
  "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-subbuild/cmake-scripts-populate-prefix/tmp"
  "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-subbuild/cmake-scripts-populate-prefix/src/cmake-scripts-populate-stamp"
  "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-subbuild/cmake-scripts-populate-prefix/src"
  "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-subbuild/cmake-scripts-populate-prefix/src/cmake-scripts-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-subbuild/cmake-scripts-populate-prefix/src/cmake-scripts-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Course/CG/cs171-hw3-2025fall-Xumu236/temp_build/_deps/cmake-scripts-subbuild/cmake-scripts-populate-prefix/src/cmake-scripts-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
