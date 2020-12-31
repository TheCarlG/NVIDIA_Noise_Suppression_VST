//------------------------------------------------------------------------
// Copyright(c) 2020 CarlG.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace NvNs {
	static const Steinberg::FUID kNvidia_Noise_SuppressionProcessorUID (0x2E6EAAF2, 0x09D85FC8, 0x8C39B013, 0x9E47B7F7);
	static const Steinberg::FUID kNvidia_Noise_SuppressionControllerUID (0xB018877B, 0x48115B76, 0x87833B79, 0xD9E3D427);

#define Nvidia_Noise_SuppressionVST3Category "Fx"
	enum NvNsParams : Steinberg::Vst::ParamID
	{
		kParamBypassId = 0,
		kParamRatioId = 1,
	};

}
