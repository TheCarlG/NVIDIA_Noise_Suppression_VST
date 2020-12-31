//------------------------------------------------------------------------
// Copyright(c) 2020 CarlG.
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace NvNs {
	class Nvidia_Noise_SuppressionController : public Steinberg::Vst::EditControllerEx1 {
		public:
			Nvidia_Noise_SuppressionController () = default;
			~Nvidia_Noise_SuppressionController () SMTG_OVERRIDE = default;

			static Steinberg::FUnknown* createInstance (void* /*context*/) {
				return (Steinberg::Vst::IEditController*)new Nvidia_Noise_SuppressionController;
			}

			// IPluginBase
			Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
			Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;

			// EditController
			Steinberg::tresult PLUGIN_API setComponentState (Steinberg::IBStream* state) SMTG_OVERRIDE;
			Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) SMTG_OVERRIDE;
			Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
			Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;
			Steinberg::tresult PLUGIN_API setParamNormalized (Steinberg::Vst::ParamID tag,
					Steinberg::Vst::ParamValue value) SMTG_OVERRIDE;
			Steinberg::tresult PLUGIN_API getParamStringByValue (Steinberg::Vst::ParamID tag,
					Steinberg::Vst::ParamValue valueNormalized,
					Steinberg::Vst::String128 string) SMTG_OVERRIDE;
			Steinberg::tresult PLUGIN_API getParamValueByString (Steinberg::Vst::ParamID tag,
					Steinberg::Vst::TChar* string,
					Steinberg::Vst::ParamValue& valueNormalized) SMTG_OVERRIDE;

			// Interface
			DEFINE_INTERFACES
				END_DEFINE_INTERFACES (EditController)
				DELEGATE_REFCOUNT (EditController)

		protected:
	};
}
