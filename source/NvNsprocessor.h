//------------------------------------------------------------------------
// Copyright(c) 2020 CarlG.
//------------------------------------------------------------------------

#pragma once
#if !defined (DEVELOPMENT) && !defined (RELEASE)
#define DEVELOPMENT 1
#endif


#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/vsttypes.h"

#include "nvAudioEffects.h"
#include <string>

namespace NvNs {
	class Nvidia_Noise_SuppressionProcessor : public Steinberg::Vst::AudioEffect {
		public:
			Nvidia_Noise_SuppressionProcessor ();
			~Nvidia_Noise_SuppressionProcessor () SMTG_OVERRIDE;

			static Steinberg::FUnknown* createInstance (void* /*context*/) {
				return (Steinberg::Vst::IAudioProcessor*)new Nvidia_Noise_SuppressionProcessor;
			}

			Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;

			Steinberg::tresult PLUGIN_API setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns, Steinberg::Vst::SpeakerArrangement* outputs, Steinberg::int32 numOuts) SMTG_OVERRIDE;

			Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;

			Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) SMTG_OVERRIDE;

			Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& newSetup) SMTG_OVERRIDE;

			Steinberg::tresult PLUGIN_API canProcessSampleSize (Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;

			Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;

			Steinberg::uint32 PLUGIN_API getLatencySamples() SMTG_OVERRIDE;


			Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
			Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

		private:
			NvAFX_Handle NvHandle{ nullptr };

			//unsigned long mBufferSizeInSamples{ 0 };
			float** mBuffer{ nullptr };
			unsigned long mBufferIdx{ 0 };

			bool mBypass{ false };
			float mRatio{ 1.0 };

			std::string modelPath;
			unsigned nvSampleSize{ 480 };
			unsigned nvNumChannels{ 2 };
			Steinberg::Vst::SampleRate nvSampleRate{ 48000 };

			bool validSampleRate(Steinberg::Vst::SampleRate sr);

	};
}
