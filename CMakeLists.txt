cmake_minimum_required(VERSION 3.14.0)

set(vst3sdk_SOURCE_DIR ../../VST3_SDK)
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(NVIDIA_Noise_Suppression)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)


smtg_enable_vst3_sdk()

smtg_add_vst3plugin(NVIDIA_Noise_Suppression
    source/version.h
    source/NvNscids.h
    source/NvNsprocessor.h
    source/NvNsprocessor.cpp
    source/NvNscontroller.h
    source/NvNscontroller.cpp
    source/NvNsentry.cpp
)

#- VSTGUI Wanted ----
if(SMTG_ADD_VSTGUI)
	target_sources(NVIDIA_Noise_Suppression
		PRIVATE
			resource/NvNseditor.uidesc
	)
    target_link_libraries(NVIDIA_Noise_Suppression
        PRIVATE
            vstgui_support
    )
    smtg_add_vst3_resource(NVIDIA_Noise_Suppression "resource/NvNseditor.uidesc")
endif()
# -------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/nvidia-nvafx/nvafx/include)
#link_directories()

find_library(NVAFX_PATH
    NAMES NVAudioEffects
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/third-party/nvidia-nvafx/nvafx
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/third-party/nvidia-nvafx/nvafx
     PATH_SUFFIXES "lib"
)
add_library(NVAudioEffects SHARED IMPORTED)
target_link_libraries(NVIDIA_Noise_Suppression
    PRIVATE
    ${NVAFX_PATH}
)
smtg_add_vst3_snapshot(NVIDIA_Noise_Suppression resource/2E6EAAF209D85FC88C39B0139E47B7F7_snapshot.png)
smtg_add_vst3_snapshot(NVIDIA_Noise_Suppression resource/2E6EAAF209D85FC88C39B0139E47B7F7_snapshot_2.0x.png)

target_link_libraries(NVIDIA_Noise_Suppression
    PRIVATE
        sdk
)

if(SMTG_MAC)
    smtg_set_bundle(NVIDIA_Noise_Suppression INFOPLIST
        "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist"
        PREPROCESS
    )
elseif(SMTG_WIN)
    target_sources(NVIDIA_Noise_Suppression PRIVATE
        resource/win32resource.rc
    )
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NVIDIA_Noise_Suppression)

	set_target_properties(NVIDIA_Noise_Suppression PROPERTIES
		VS_DEBUGGER_COMMAND
			"$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
		VS_DEBUGGER_COMMAND_ARGUMENTS
			" --pluginfolder \"$(OutDir)\" "
	)
endif()

