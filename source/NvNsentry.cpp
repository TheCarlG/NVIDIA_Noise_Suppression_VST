//------------------------------------------------------------------------
// Copyright(c) 2020 CarlG.
//------------------------------------------------------------------------

#include "NvNsprocessor.h"
#include "NvNscontroller.h"
#include "NvNscids.h"
#include "version.h"

#include "public.sdk/source/main/pluginfactory.h"

#define stringPluginName "NVIDIA Noise Suppression"

bool InitModule () {
	return true;
}

bool DeinitModule () {
	return true;
}

using namespace Steinberg::Vst;
using namespace NvNs;

BEGIN_FACTORY_DEF ("CarlG",
		"https://www.karatefylla.nu",
		"mailto:calle.gunnarsson@gmail.com")

	DEF_CLASS2 (INLINE_UID_FROM_FUID(kNvidia_Noise_SuppressionProcessorUID),
		PClassInfo::kManyInstances,
		kVstAudioEffectClass,
		stringPluginName,
		Vst::kDistributable,
		Nvidia_Noise_SuppressionVST3Category,
		FULL_VERSION_STR,
		kVstVersionString,
		Nvidia_Noise_SuppressionProcessor::createInstance)

	DEF_CLASS2 (INLINE_UID_FROM_FUID (kNvidia_Noise_SuppressionControllerUID),
			PClassInfo::kManyInstances,
			kVstComponentControllerClass,
			stringPluginName "Controller",
			0,
			"",
			FULL_VERSION_STR,
			kVstVersionString,
			Nvidia_Noise_SuppressionController::createInstance)
END_FACTORY
