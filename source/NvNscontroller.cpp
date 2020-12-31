//------------------------------------------------------------------------
// Copyright(c) 2020 CarlG.
//------------------------------------------------------------------------

#include "NvNscontroller.h"
#include "NvNscids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"


using namespace Steinberg;

namespace NvNs {
	tresult PLUGIN_API Nvidia_Noise_SuppressionController::initialize (FUnknown* context) {
		tresult result = EditControllerEx1::initialize (context);
		if (result != kResultOk) {
			return result;
		}

		parameters.addParameter(STR16("Bypass"), nullptr, 1, 0, Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass, kParamBypassId);
		parameters.addParameter(STR16("Ratio"), nullptr, 100, 1.0, Vst::ParameterInfo::kCanAutomate, kParamRatioId);

		return result;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionController::terminate () {
		return EditControllerEx1::terminate ();
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionController::setComponentState (IBStream* state) {
		if (!state)
			return kResultFalse;

		IBStreamer streamer(state, kLittleEndian);

		int32 bypassState;
		if (streamer.readInt32(bypassState) == false)
			return kResultFalse;
		setParamNormalized(kParamBypassId, bypassState ? 1 : 0);

		float ratioState = 1.0f;
		if (streamer.readFloat(ratioState) == false)
			return kResultFalse;
		setParamNormalized(kParamRatioId, ratioState);

		return kResultOk;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionController::setState (IBStream* state) {
		return kResultTrue;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionController::getState (IBStream* state) {
		return kResultTrue;
	}

	IPlugView* PLUGIN_API Nvidia_Noise_SuppressionController::createView (FIDString name) {
		if (FIDStringsEqual (name, Vst::ViewType::kEditor)) {
			return nullptr;
		}
		return nullptr;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionController::setParamNormalized (Vst::ParamID tag, Vst::ParamValue value) {
		tresult result = EditControllerEx1::setParamNormalized (tag, value);
		return result;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionController::getParamStringByValue (Vst::ParamID tag, Vst::ParamValue valueNormalized, Vst::String128 string) {
		return EditControllerEx1::getParamStringByValue (tag, valueNormalized, string);
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionController::getParamValueByString (Vst::ParamID tag, Vst::TChar* string, Vst::ParamValue& valueNormalized) {
		return EditControllerEx1::getParamValueByString (tag, string, valueNormalized);
	}
}
