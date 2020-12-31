//------------------------------------------------------------------------
// Copyright(c) 2020 CarlG.
//------------------------------------------------------------------------
#if !defined(DEVELOPMENT) && !defined(RELEASE)
#define DEVELOPMENT 1
#endif
#include "NvNsprocessor.h"
#include "NvNscids.h"
#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"

const char* MODEL = "\\models\\denoiser_48k.trtpkg";

const unsigned DEAFULT_SAMPLE_RATE = 48000;

using namespace Steinberg;

namespace NvNs {
	Nvidia_Noise_SuppressionProcessor::Nvidia_Noise_SuppressionProcessor()
	{
		setControllerClass(kNvidia_Noise_SuppressionControllerUID);
	}

	Nvidia_Noise_SuppressionProcessor::~Nvidia_Noise_SuppressionProcessor() {}

	tresult PLUGIN_API Nvidia_Noise_SuppressionProcessor::initialize(FUnknown* context)
	{
		auto result = AudioEffect::initialize(context);
		if (result != kResultOk) {
			return result;
		}

		addAudioInput(STR16("AudioInput"), Vst::SpeakerArr::kMono);
		addAudioOutput(STR16("AudioOutput"), Vst::SpeakerArr::kMono);

		auto sdk_path = std::string(std::getenv("NVAFX_SDK_DIR"));
		if (sdk_path.length() == 0) {
			printf("NVAFX_SDK_DIR not set\n");
			return kResultFalse;
		}

		modelPath = sdk_path + MODEL;
		if (NvHandle == nullptr) {
			auto err = NvAFX_CreateEffect(NVAFX_EFFECT_DENOISER, &NvHandle);
			if (err != NVAFX_STATUS_SUCCESS) {
				FDebugPrint("NvAFX_CreateEffect(%s, %p) failed: %d\n", NVAFX_EFFECT_DENOISER, &NvHandle, err);
				return kResultFalse;
			}
			//FDebugPrint("NvAFX_CreateEffect(%s, %p) succeeded: %p\n", NVAFX_EFFECT_DENOISER, &NvHandle, NvHandle);

			err = NvAFX_SetString(NvHandle, NVAFX_PARAM_DENOISER_MODEL_PATH, modelPath.c_str());
			if (err != NVAFX_STATUS_SUCCESS) {
				FDebugPrint("NvAFX_SetString(%p, %s, '%s') failed: %d\n", NvHandle, NVAFX_PARAM_DENOISER_MODEL_PATH,
							modelPath.c_str(), err);
				return kResultFalse;
			}
			//FDebugPrint("NvAFX_SetString(%p, %s, '%s') succeeded\n", NvHandle, NVAFX_PARAM_DENOISER_MODEL_PATH,
			//modelPath.c_str());

			err = NvAFX_SetU32(NvHandle, NVAFX_PARAM_DENOISER_SAMPLE_RATE, DEAFULT_SAMPLE_RATE);
			if (err != NVAFX_STATUS_SUCCESS) {
				FDebugPrint("NvAFX_SetU32(%p, %s, %d) failed: %d\n", NvHandle, NVAFX_PARAM_DENOISER_SAMPLE_RATE,
							DEAFULT_SAMPLE_RATE, err);
				return kResultFalse;
			}
			//FDebugPrint("NvAFX_SetU32(%p, %s, %d) succeeded\n", NvHandle, NVAFX_PARAM_DENOISER_SAMPLE_RATE,
			//DEAFULT_SAMPLE_RATE);

			if (NvAFX_SetFloat(NvHandle, NVAFX_PARAM_DENOISER_INTENSITY_RATIO, 1.0) != NVAFX_STATUS_SUCCESS) {
				FDebugPrint("NvAFX_SetFloat(%p, %s, %f) failed: %d\n", NvHandle, NVAFX_PARAM_DENOISER_INTENSITY_RATIO,
							1.0, err);
				return kResultFalse;
			}
			//FDebugPrint("NvAFX_SetFloat(%p, %s, %f) succeeded\n", NvHandle, NVAFX_PARAM_DENOISER_INTENSITY_RATIO, 1.0);

			err = NvAFX_Load(NvHandle);
			if (err != NVAFX_STATUS_SUCCESS) {
				FDebugPrint("NvAFX_Load(%p) failed: %d\n", NvHandle, err);
				return kResultFalse;
			}
			//FDebugPrint("NvAFX_Load(%p) succeeded\n", NvHandle);
		}

		return kResultOk;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionProcessor::setBusArrangements(Vst::SpeakerArrangement* inputs,
																			 int32                    numIns,
																			 Vst::SpeakerArrangement* outputs,
																			 int32                    numOuts)
	{
		if (numIns == 1 && numOuts == 1 && inputs[0] == Vst::SpeakerArr::kMono
			&& outputs[0] == Vst::SpeakerArr::kMono) {
			return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
		}

		return kResultFalse;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionProcessor::terminate()
	{
		if (NvHandle != nullptr) {
			auto err = NvAFX_DestroyEffect(NvHandle);
			if (err != NVAFX_STATUS_SUCCESS) {
				FDebugPrint("NvAFX_DestroyEffect(%p) failed: %d\n", NvHandle, err);
			} else {
				//FDebugPrint("NvAFX_DestroyEffect(%p) succeeded\n", NvHandle);
			}
			NvHandle = nullptr;
		}

		return AudioEffect::terminate();
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionProcessor::setActive(TBool state)
	{
		if (NvHandle != nullptr) {
			auto newSr = DEAFULT_SAMPLE_RATE;
			if (validSampleRate(nvSampleRate)) {
				newSr = (unsigned int)nvSampleRate;
			}

			auto err = NvAFX_SetU32(NvHandle, NVAFX_PARAM_DENOISER_SAMPLE_RATE, newSr);
			if (err != NVAFX_STATUS_SUCCESS) {
				FDebugPrint("NvAFX_SetU32(%p, %s, %d) failed: %d\n", NvHandle, NVAFX_PARAM_DENOISER_SAMPLE_RATE, newSr,
							err);
				return kResultFalse;
			}
			//FDebugPrint("NvAFX_SetU32(%p, %s, %d) succeeded\n", NvHandle, NVAFX_PARAM_DENOISER_SAMPLE_RATE, newSr);

			err = NvAFX_SetFloat(NvHandle, NVAFX_PARAM_DENOISER_INTENSITY_RATIO, mRatio);
			if (err != NVAFX_STATUS_SUCCESS) {
				FDebugPrint("NvAFX_SetFloat(%p, %s, %f) failed: %d\n", NvHandle, NVAFX_PARAM_DENOISER_INTENSITY_RATIO,
							mRatio, err);
				return kResultFalse;
			}
			//FDebugPrint("NvAFX_SetFloat(%p, %s, %f) succeeded\n", NvHandle, NVAFX_PARAM_DENOISER_INTENSITY_RATIO,
			//mRatio);

			if (state) {
				mBuffer = (float**)std::malloc(nvNumChannels * sizeof(float*));

				auto size = (size_t)(Vst::getSampleFramesSizeInBytes(processSetup, nvSampleSize));
				for (unsigned ch = 0; ch < nvNumChannels; ch++) {
					mBuffer[ch] = (float*)std::malloc(size);
					if (mBuffer[ch]) {
						memset(mBuffer[ch], 0, size);
					}
				}

				mBufferIdx = 0;
			} else {
				if (mBuffer != nullptr) {
					for (unsigned ch = 0; ch < nvNumChannels; ch++) {
						std::free(mBuffer[ch]);
					}
					std::free(mBuffer);
					mBuffer    = nullptr;
					mBufferIdx = 0;
				}
			}
		}

		return AudioEffect::setActive(state);
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionProcessor::process(Vst::ProcessData& data)
	{
		if (data.inputParameterChanges != nullptr) {
			int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
			for (int32 index = 0; index < numParamsChanged; index++) {
				if (Vst::IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData(index)) {
					Vst::ParamValue value;
					int32           sampleOffset;
					int32           numPoints = paramQueue->getPointCount();
					switch (paramQueue->getParameterId()) {
					case NvNsParams::kParamBypassId:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
							mBypass = (value > 0.5f);
						}
						break;
					case NvNsParams::kParamRatioId:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
							mRatio = value;
						}
						break;
					}
				}
			}
		}

		if (data.numInputs == 0 || data.numOutputs == 0 || data.numSamples == 0) {
			return kResultOk;
		}

		uint32  sampleFrameBytes = getSampleFramesSizeInBytes(processSetup, data.numSamples);
		float** in               = (float**)getChannelBuffersPointer(processSetup, data.inputs[0]);
		float** out              = (float**)getChannelBuffersPointer(processSetup, data.outputs[0]);

		if (data.inputs[0].silenceFlags != 0) {
			data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;
			for (unsigned ch = 0; ch < nvNumChannels; ch++) {
				if (in[ch] != out[ch]) {
					memset(out[ch], 0, sampleFrameBytes);
				}
			}

			return kResultOk;
		}

		if (mBypass || (unsigned)nvSampleRate != DEAFULT_SAMPLE_RATE) {
			for (unsigned ch = 0; ch < nvNumChannels; ch++) {
				if (in[ch] != out[ch]) {
					memcpy(out[ch], in[ch], sampleFrameBytes);
				}
			}

			return kResultOk;
		}

		// nvSampleSize is 480
		// nvChannelNum is 1
		// We need to use a ring buffer since NvAFX_Run() needs 480 samples
		if (mBuffer != nullptr) {
			int overflow = (mBufferIdx + data.numSamples) - nvSampleSize;
			if (overflow < 0)
				overflow = 0;

			int  upperIdx = data.numSamples - overflow;
			auto size     = getSampleFramesSizeInBytes(processSetup, upperIdx);

			//FDebugPrint("Reading %d samples and writing %d(%d overflow) to mBuffer idx %d new len %d\n",
			//data.numSamples, upperIdx, overflow, mBufferIdx, mBufferIdx + upperIdx);

			if (upperIdx > 0) {
				for (unsigned ch = 0; ch < nvNumChannels; ch++) {
					if (mBuffer[ch]) {
						memcpy(mBuffer[ch] + mBufferIdx, in[ch], size);
					}
				}
			}

			mBufferIdx += data.numSamples;

			if (overflow > 0) {
				/*
				// For testing instead of running NvAFX_Run(), just copy everything from mBuffer to out
				auto size = getSampleFramesSizeInBytes(processSetup, nvSampleSize);
				for (unsigned ch = 0; ch < nvNumChannels; ch++) {
					if (mBuffer[ch]) {
						memcpy(out[ch], mBuffer[ch], size);
					}
				}
				//*/

				auto err = NvAFX_Run(NvHandle, (const float**)mBuffer, out, nvSampleSize, nvNumChannels);
				if (err != NVAFX_STATUS_SUCCESS) {
					FDebugPrint("NvAFX_Run(%p, %p, %p, %d, %d) failed: %d\n", NvHandle, mBuffer, out, nvSampleSize,
								nvNumChannels, err);
					return kResultFalse;
				}
				//*/

				mBufferIdx = overflow;
				size       = getSampleFramesSizeInBytes(processSetup, overflow);
				for (unsigned ch = 0; ch < nvNumChannels; ch++) {
					if (mBuffer[ch]) {
						memcpy(mBuffer[ch], in[ch] + upperIdx, size);
					}
				}
			}
		}

		return kResultOk;
	}

	bool Nvidia_Noise_SuppressionProcessor::validSampleRate(Vst::SampleRate sr)
	{
		unsigned     sample_rate;
		NvAFX_Status err;
		err = NvAFX_GetU32(NvHandle, NVAFX_PARAM_DENOISER_NUM_SAMPLES_PER_FRAME, &sample_rate);
		if (err != NVAFX_STATUS_SUCCESS) {
			FDebugPrint("NvAFX_GetU32(%p, %s, %p) failed: %d\n", NvHandle, NVAFX_PARAM_DENOISER_SAMPLE_RATE,
						&sample_rate, err);
			return false;
		}
		//FDebugPrint("NvAFX_GetU32(%p, %s, %p) success\n", NvHandle, NVAFX_PARAM_DENOISER_SAMPLE_RATE, &sample_rate);

		return (unsigned)sr == sample_rate;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionProcessor::setupProcessing(Vst::ProcessSetup& newSetup)
	{
		auto err = NvAFX_GetU32(NvHandle, NVAFX_PARAM_DENOISER_NUM_SAMPLES_PER_FRAME, &nvSampleSize);
		if (err != NVAFX_STATUS_SUCCESS) {
			FDebugPrint("NvAFX_GetU32(%p, %s, %p) failed: %d\n", NvHandle, NVAFX_PARAM_DENOISER_NUM_SAMPLES_PER_FRAME,
						&nvSampleSize, err);
			return kResultFalse;
		}
		//FDebugPrint("NvAFX_GetU32(%p, %s, %p) = %d: success\n", NvHandle, NVAFX_PARAM_DENOISER_NUM_SAMPLES_PER_FRAME,
		//&nvSampleSize, nvSampleSize);

		err = NvAFX_GetU32(NvHandle, NVAFX_PARAM_DENOISER_NUM_CHANNELS, &nvNumChannels);
		if (err != NVAFX_STATUS_SUCCESS) {
			FDebugPrint("NvAFX_GetU32(%p, %s, %p) failed: %d\n", NvHandle, NVAFX_PARAM_DENOISER_NUM_CHANNELS,
						&nvNumChannels, err);
			return kResultFalse;
		}
		//FDebugPrint("NvAFX_GetU32(%p, %s, %p) = %d: success\n", NvHandle, NVAFX_PARAM_DENOISER_NUM_CHANNELS,
		//&nvNumChannels, nvNumChannels);

		nvSampleRate                = newSetup.sampleRate;
		newSetup.maxSamplesPerBlock = nvSampleSize;

		return AudioEffect::setupProcessing(newSetup);
	}

	uint32 PLUGIN_API Nvidia_Noise_SuppressionProcessor::getLatencySamples()
	{
		return nvSampleSize;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionProcessor::canProcessSampleSize(int32 symbolicSampleSize)
	{
		if (symbolicSampleSize == Vst::kSample32) {
			return kResultTrue;
		}

		return kResultFalse;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionProcessor::setState(IBStream* state)
	{
		if (state == nullptr) {
			return kResultFalse;
		}

		IBStreamer streamer(state, kLittleEndian);

		int32 savedBypass = 0;
		if (streamer.readInt32(savedBypass) == false) {
			return kResultFalse;
		}

		mBypass = savedBypass > 0;

		float savedRatio = 0;
		if (streamer.readFloat(savedRatio) == false) {
			return kResultFalse;
		}
		mRatio = savedRatio;

		return kResultOk;
	}

	tresult PLUGIN_API Nvidia_Noise_SuppressionProcessor::getState(IBStream* state)
	{
		int32 toSaveBypass = mBypass ? 1 : 0;

		IBStreamer streamer(state, kLittleEndian);
		streamer.writeInt32(toSaveBypass);
		streamer.writeFloat(mRatio);

		return kResultOk;
	}
} // namespace NvNs
