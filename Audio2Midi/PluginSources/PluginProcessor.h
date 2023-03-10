#pragma once

#include "atomic"
#include <JuceHeader.h>

#include "DownSampler.h"
#include "ProcessorBase.h"

struct UIParameters
{
    std::atomic<float> noteSegmentationThreshold = 0.5;
    std::atomic<float> modelConfidenceThreshold = 0.5;
    std::atomic<float> minNoteDurationMs = 11;
    std::atomic<bool> recordOn = false;
};

enum State
{
    EmptyAudioAndMidiRegions = 0,
    Recording,
    Processing,
    PopulatedAudioAndMidiRegions
};

class Audio2MidiAudioProcessor : public PluginHelpers::ProcessorBase
{
public:
    Audio2MidiAudioProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    State getState() const { return mState.load(); };

    void clear();

    AudioBuffer<float>& getAudioBufferForMidi();

    int getNumSamplesAcquired();

    void setNumSamplesAcquired(int inNumSamplesAcquired);

    UIParameters mParameters;

private:
    DownSampler mDownSampler;

    std::atomic<State> mState = EmptyAudioAndMidiRegions;

    AudioBuffer<float> mAudioBufferForMIDITranscription;

    juce::ThreadPool mThreadPool;

    int mNumSamplesAcquired = 0;
    const double mBasicPitchSampleRate = 22050.0;
    const double mMaxDuration = 3 * 60;
    const int mMaxNumSamplesToConvert =
        static_cast<int>(mBasicPitchSampleRate * mMaxDuration);
};
