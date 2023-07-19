macro(set_conan_deps)
include(FetchContent)

set(CONAN_CMAKE_UPDATED OFF)

# Check if cached conan.cmake file is up-to-date
set(CONAN_CMAKE_HASH "C2925BFB443AF3C333E9C27221CA1F7ED529B7BBEB74E70397C1DD3342552891")
set(CONAN_CMAKE_GIT_TAG "04ab73eb826c3884d59687f37c32824119cbf1c6")
set(CONAN_CMAKE_CURRENT_HASH "")


if(EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
	file(SHA256 "${CMAKE_BINARY_DIR}/conan.cmake" CONAN_CMAKE_CURRENT_HASH)
	string(TOUPPER ${CONAN_CMAKE_CURRENT_HASH} CONAN_CMAKE_CURRENT_HASH)
	if(NOT CONAN_CMAKE_CURRENT_HASH STREQUAL CONAN_CMAKE_HASH)
		message(STATUS "Conan: Downloading the newest conan.cmake from https://github.com/xlabmedical/conan-cmake")
		file(REMOVE "${conan_cmake_SOURCE_DIR}/conan.cmake")
		# Download from XLAB github repo
		FetchContent_Declare(conan_cmake
			GIT_REPOSITORY https://github.com/xlabmedical/conan-cmake
			GIT_TAG ${CONAN_CMAKE_GIT_TAG}
		)
		FetchContent_MakeAvailable(conan_cmake)
		file(COPY "${conan_cmake_SOURCE_DIR}/conan.cmake" DESTINATION "${CMAKE_BINARY_DIR}")

		set(CONAN_CMAKE_UPDATED ON)
	endif()
else()
	message(STATUS "Conan: Downloading the newest conan.cmake from https://github.com/xlabmedical/conan-cmake")
	# Download from XLAB github repo
	FetchContent_Declare(conan_cmake
		GIT_REPOSITORY https://github.com/xlabmedical/conan-cmake
		GIT_TAG ${CONAN_CMAKE_GIT_TAG}
	)
	FetchContent_MakeAvailable(conan_cmake)
	file(COPY "${conan_cmake_SOURCE_DIR}/conan.cmake" DESTINATION "${CMAKE_BINARY_DIR}")

	set(CONAN_CMAKE_UPDATED ON)
endif()
include("${CMAKE_BINARY_DIR}/conan.cmake")

if(CONAN_CMAKE_UPDATED)
	# Force update remotes
	message(STATUS "Conan: Updating remotes...")
	conan_add_remotes()
endif()

# Scan for conan recipe in the root source directory.
# If the conan recipe ('conanfile.py') doesn't exist in
# the root source directory, then the process will stop and return an error.
set(CONAN_RECIPE_FILE "")
if(EXISTS "${CMAKE_SOURCE_DIR}/conanfile.py")
	set(CONAN_RECIPE_FILE "${CMAKE_SOURCE_DIR}/conanfile.py")
endif()
if(CONAN_RECIPE_FILE STREQUAL "")
  message(FATAL_ERROR "Conan recipe doesn't exist in the root source directory...")
endif()
file(SHA256 ${CONAN_RECIPE_FILE} CONAN_FILE_HASH)


# Check if conanfile.py was updated
if(CONAN_FILE_HASH STREQUAL CONAN_FILE_HASH_CACHE AND EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake)
	include(${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake)
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_BINARY_DIR})
else()
	# Remove cppstd entry from autodetected settings
	conan_cmake_autodetect(settings)
	string(REGEX REPLACE "compiler.cppstd=.." "" settings "${settings}")

	# Write profile to temporary file
	set(CONAN_PROFILE_FN "${CMAKE_BINARY_DIR}/conan_profile")
	set(CONAN_PROFILE "")
	string(APPEND CONAN_PROFILE "[settings]\n")

	if(WIN32)
		string(APPEND CONAN_PROFILE "os=Windows\n")
	elseif(APPLE)
		string(APPEND CONAN_PROFILE "os=Macos\n")
	endif()
	string(APPEND CONAN_PROFILE "arch=x86_64\n")

	string(APPEND CONAN_PROFILE "${settings}")
	string(REGEX REPLACE ";" "\n" CONAN_PROFILE "${CONAN_PROFILE}")

	string(APPEND CONAN_PROFILE "\n[conf]\n")
	string(APPEND CONAN_PROFILE "tools.cmake.cmaketoolchain:generator=${CMAKE_GENERATOR}\n")
	file(WRITE ${CONAN_PROFILE_FN} ${CONAN_PROFILE})

	# Install packages
	conan_cmake_install(
		PATH_OR_REFERENCE "${CONAN_RECIPE_FILE}"
		OUTPUT_FOLDER     "${CMAKE_BINARY_DIR}"
		UPDATE
		PROFILE           ${CONAN_PROFILE_FN}
		PROFILE_HOST	  ${CONAN_PROFILE_FN}
		PROFILE_BUILD     ${CONAN_PROFILE_FN}
		BUILD zlib
	)
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_BINARY_DIR})
	if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake)
		include(${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake)
	endif()

	# Write conan file hash to cache
	set(CONAN_FILE_HASH_CACHE ${CONAN_FILE_HASH} CACHE STRING "SHA256 hash of the conanfile" FORCE)
endif()


endmacro()
