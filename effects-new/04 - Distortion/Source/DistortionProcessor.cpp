#include "DistortionProcessor.h"
#include "DistortionEditor.h"
#include "DistortionType.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* DistortionProcessor::createEditor()
{
    return new DistortionEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
DistortionProcessor::DistortionProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, &undoManager, Identifier(JucePlugin_Name), DistortionParameters::createParameterLayout())
    , parameters(valueTreeState)
{
}

// Destructor
DistortionProcessor::~DistortionProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void DistortionProcessor::prepareToPlay (double /*sampleRate*/, int /*maxSamplesPerBlock*/)
{
}

// Audio processing finished; release any allocated memory
void DistortionProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void DistortionProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    // apply the same gain factor to all input channels for which there is an output channel
    
    for (int channelIndex = 0; channelIndex < getTotalNumInputChannels(); channelIndex++)
    {
        const float* pIn = buffer.getReadPointer(channelIndex);
        float* pOut = buffer.getWritePointer(channelIndex);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            *pOut++ = *pIn++ * parameters.linearGain;
            
        
        }
    }
    // clear any remaining/excess output channels to zero
    for (; channelIndex < getTotalNumOutputChannels(); channelIndex++)
    {
        buffer.clear(channelIndex, 0, buffer.getNumSamples());
    }
}

// Called by the host when it needs to persist the current plugin state
void DistortionProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void DistortionProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
