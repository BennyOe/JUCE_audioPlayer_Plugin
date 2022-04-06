/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PlayerPluginAudioProcessorEditor
        : public juce::AudioProcessorEditor,
          public juce::ChangeListener,
          public juce::FileDragAndDropTarget{
public:
    PlayerPluginAudioProcessorEditor(PlayerPluginAudioProcessor &);

    ~PlayerPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;

    void resized() override;


private:
    void changeListenerCallback(juce::ChangeBroadcaster* source) override {
        if (source == &audioProcessor.transport) transportSourceChanged();
        if (source == &thumbnail) thumbnailChanged();
    }

    void transportSourceChanged();

    void thumbnailChanged();

    static void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);

    void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);

    void openButton1Clicked();

    void playButtonClicked();

    void stopButtonClicked();

    void loopButtonClicked();

    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

    juce::TextButton openButton1{"Open 1"};
    juce::TextButton playButton{"Play"};
    juce::TextButton stopButton{"Stop"};
    juce::ToggleButton loopButton{"Loop"};
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;


    std::unique_ptr<juce::FileChooser> chooser1;


    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PlayerPluginAudioProcessor &audioProcessor;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerPluginAudioProcessorEditor)
};
