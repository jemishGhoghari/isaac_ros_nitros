/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef NVIDIA_GXF_CORE_GXF_EXT_H_
#define NVIDIA_GXF_CORE_GXF_EXT_H_

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------------------------------------------

/// @brief GXF bitmasks
typedef uint32_t GxfFlags;

// -------------------------------------------------------------------------------------------------

/// @brief Structure specifing parameters for loading extensions
typedef struct {
  // Optional list of extension filenames to load
  const char* const* extension_filenames;
  // The number of extensions to load
  uint32_t extension_filenames_count;
  // Optional list of manifest filenames to load
  const char* const* manifest_filenames;
  // The number of manifests to load
  uint32_t manifest_filenames_count;
  // An optional base directory which is prepended to all extensions filenames, including those
  // loaded via manifests.
  const char* base_directory;
} GxfLoadExtensionsInfo;

/// @brief Loads GXF extension libraries
///
/// Loads one or more extensions either directly by their filename or indirectly by loading
/// manifest files. Before a component can be added to a GXF entity the GXF extension shared
/// library providing the component must be loaded. An extensions must only be loaded once.
///
/// To simplify loading multiple extensions at once the developer can create a manifest file which
/// lists all extensions he needs. This function will then load all extensions listed in the
/// manifest file. Multiple manifest may be loaded, however each extensions may still be loaded
/// only a single time.
///
/// A manifest file is a YAML file with a single top-level entry 'extensions' followed by a
/// list of filenames of GXF extension shared libraries.
///
/// Example:
/// -----  START OF FILE  -----
/// extensions:
/// - gxf/std/libgxf_std.so
/// - gxf/npp/libgxf_npp.so
/// -----   END OF FILE   -----
///
/// @param context is the GXF context in which to load extensions
/// @param info is a pointer to a GxfLoadExtensionsInfo structure describing parameters for loading
/// @return GXF_SUCCESS if the operation was successful, or otherwise one of the GXF error codes.
gxf_result_t GxfLoadExtensions(gxf_context_t context, const GxfLoadExtensionsInfo* info);

/// @brief Loads a metadata file generated by the nvgraph_registry
///
/// The nvgraph_registry tool generates a metadata file of the contents of an extension during
/// registration. These metadata files can be used to resolve typename and TID's of components for
/// other extensions which depend on them. metadata files do not contain the actual implementation
/// of the extension and must be loaded only to run the extension query API's on extension
/// libraries which have the actual implementation and only depend on the metadata for type
/// resolution.
///
/// If some components of extension B depend on some components in extension A:
/// - Load metadata file for extension A
/// - Load extension library for extension B using 'GxfLoadExtensions'
/// - Run extension query api's on extension B and it's components.
///
/// @param context A valid GXF context
/// @param filenames absolute paths of metadata files generated by the registry during
///                  extension registration
/// @param count The number of metadata files to be loaded
/// @return GXF_SUCCESS if the operation was successful, or otherwise one of the GXF error codes.
gxf_result_t GxfLoadExtensionMetadataFiles(gxf_context_t context, const char* const* filenames,
                                           uint32_t count);

// -------------------------------------------------------------------------------------------------

/// @brief Bitmask specifying storage mode for an entity
typedef enum GxfEntityCreateFlagBits {
  // 'GXF_ENTITY_CREATE_PROGRAM_BIT' specifies that the entity will be added to the program
  // entities. Program entities are kept alive for the duration of the program. They are activated
  // automatically when the program is activated and deactivated when the program is deactivated.
  // If the program was already actived when the entity is created the entity must still be
  // activated manually.
  GXF_ENTITY_CREATE_PROGRAM_BIT = 0x00000001
} GxfEntityCreateFlagBits;

/// @brief Bitmask of GxfEntityCreateFlagBits
typedef GxfFlags GxfEntityCreateFlags;

/// @brief Structure specifing parameters for creating entities
typedef struct {
  // 'entity_name' is the name of the entity which is created. If this is a nullptr an undefined
  // unique name is chosen. The name must not start with a double underscore.
  const char* entity_name;
  // 'flags' is a bitmask of GxfEntityCreateFlagsBits indicating storage method and usage behavior
  // for the created entity.
  GxfEntityCreateFlags flags;
} GxfEntityCreateInfo;

/// @brief Create a new GXF entity
///
/// Entities are light-weight containers to hold components and form the basic building blocks
/// of a GXF application. Entities are created when a GXF file is loaded, or they can be created
/// manually using this function. Entities created with this function must be destroyed using
/// 'GxfEntityDestroy'. After the entity was created components can be added to it with
/// 'GxfComponentAdd'. To start execution of codelets on an entity the entity needs to be
/// activated first. This can happen automatically using 'GXF_ENTITY_CREATE_PROGRAM_BIT' or
/// manually using 'GxfEntityActivate'.
///
/// @param context is the GXF context that creates the entity.
/// @param info is a pointer to a GxfEntityCreateInfo structure containing parameters affecting
///             the creation of the entity.
/// @param eid is a pointer to a gxf_uid_t handle in which the resulting entity is returned.
/// @return On success the function returns GXF_SUCCESS.
gxf_result_t GxfCreateEntity(gxf_context_t context, const GxfEntityCreateInfo* info,
                             gxf_uid_t* eid);

// -------------------------------------------------------------------------------------------------

/// @brief Severity levels for GXF_LOG_* logging macros
typedef enum {
  GXF_SEVERITY_NONE = 0,
  GXF_SEVERITY_ERROR = 1,
  GXF_SEVERITY_WARNING = 2,
  GXF_SEVERITY_INFO = 3,
  GXF_SEVERITY_DEBUG = 4,
} gxf_severity_t;

/// @brief Sets the severity level of the logs (corresponding to GXF_LOG_* logging macros) for the
/// entire application
///
/// @param context a valid GXF context
/// @param severity a valid severity level as defined in `gxf_severity_t`. Logs corresponding to
///                 any level <= severity will be logged.
/// @return On success the function returns GXF_SUCCESS.
///
gxf_result_t GxfSetSeverity(gxf_context_t context, gxf_severity_t severity);

// -------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // NVIDIA_GXF_CORE_GXF_EXT_H_
