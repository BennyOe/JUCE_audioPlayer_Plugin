/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PlayerPluginAudioProcessorEditor::PlayerPluginAudioProcessorEditor(PlayerPluginAudioProcessor &p)
        : AudioProcessorEditor(&p),
          audioProcessor(p),
          thumbnailCache(5),
          thumbnail(512, audioProcessor.formatManager, thumbnailCache) {
    thumbnail.addChangeListener(this);

    // setup buttons
    auto purpleHue = juce::Colours::purple.getHue();

    addAndMakeVisible(&openButton1);
    openButton1.onClick = [this] { openButton1Clicked(); };


    addAndMakeVisible(&playButton);
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId,
                         juce::Colour::fromHSV(purpleHue, 0.5f, 0.7f, 1.0f));
    playButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId,
                         juce::Colour::fromHSV(purpleHue, 0.5f, 0.3f, 1.0f));
    stopButton.setEnabled(false);

    addAndMakeVisible(loopButton);
    loopButton.onClick = [this] { loopButtonClicked(); };

    setSize(400, 400);

}

PlayerPluginAudioProcessorEditor::~PlayerPluginAudioProcessorEditor() {
}

//==============================================================================
void PlayerPluginAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    juce::Rectangle<int> thumbnailBounds(10, 100, getWidth() - 20, getHeight() - 120);

    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void PlayerPluginAudioProcessorEditor::resized() {
    openButton1.setBounds(25, 20, getWidth() - 50, 50);
    stopButton.setBounds(10, getHeight() - 50, 60, 40);
    playButton.setBounds(80, getHeight() - 50, 60, 40);
    loopButton.setBounds(getWidth() - 80, getHeight() - 50, 60, 40);
}

void PlayerPluginAudioProcessorEditor::openButton1Clicked() {
    chooser1 = std::make_unique<juce::FileChooser>(
            "11Select a Wave file to play...",
            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
            "*.wav; *.mp3"); // [7]
    auto chooserFlags = juce::FileBrowserComponent::openMode |
                        juce::FileBrowserComponent::canSelectFiles;

    chooser1->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();
        audioProcessor.loadFileIntoTransport(file);
        playButton.setEnabled(true);
        thumbnail.setSource(new juce::FileInputSource(file));
    });
}

void PlayerPluginAudioProcessorEditor::playButtonClicked() {
    audioProcessor.transport.start();
    stopButton.setEnabled(true);
    playButton.setEnabled(false);
}

void PlayerPluginAudioProcessorEditor::stopButtonClicked() {
    audioProcessor.transport.stop();
    stopButton.setEnabled(false);
    audioProcessor.transport.setPosition(0.0);
    playButton.setEnabled(true);
}

void PlayerPluginAudioProcessorEditor::loopButtonClicked() {
    auto state = loopButton.getToggleState();
//    DBG(std::to_string(state));
    audioProcessor.currentAudioFileSource->setLooping(state);
}


void PlayerPluginAudioProcessorEditor::transportSourceChanged() {
}

void PlayerPluginAudioProcessorEditor::thumbnailChanged() {
    repaint();
}

void
PlayerPluginAudioProcessorEditor::paintIfNoFileLoaded(juce::Graphics &g, const juce::Rectangle<int> &thumbnailBounds) {
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void
PlayerPluginAudioProcessorEditor::paintIfFileLoaded(juce::Graphics &g, const juce::Rectangle<int> &thumbnailBounds) {
    g.setColour(juce::Colours::white);
    g.fillRect(thumbnailBounds);

    g.setColour(juce::Colours::darkgrey);

    thumbnail.drawChannels(g,
                           thumbnailBounds,
                           0.0,                                    // start time
                           thumbnail.getTotalLength(),             // end time
                           1.0f);                                  // vertical zoom
}