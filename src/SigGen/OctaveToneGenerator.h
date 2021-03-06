////////////////////////////////////////////////////////////////////////////////////////
// ADCTest Copyright and License
// ADCTest is in the public domain and is licensed under a 3-Clause BSD license
////////////////////////////////////////////////////////////////////////////////////////

#ifndef OCTAVETONEGENERATOR_H
#define OCTAVETONEGENERATOR_H

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <vector>

#include "WavFileWriter.h"

class OctaveToneGenerator
{
    public:
        OctaveToneGenerator(double sampleRate, int channels);
		OctaveToneGenerator(double pbSampleRate, double recSampleRate, int channels);
        virtual ~OctaveToneGenerator();

		void Create(double pbSampleRate, double recSampleRate, int channels);
		bool generateSignal(wxXmlNode* parameters);

	protected:
		void generateFrequenciesList();
		void setParameters( wxXmlNode* paramsNode );
		bool writeSignalFile();

    protected:
        wxXmlNode* mParamsNode;
		double mPbSampleRate;
		double mRecSampleRate;
		int mNoChannels;
		wxString mFolderPath;
		wxString mFileName;
		wxString mFilePath;

    private:
		int mSelectedChannelIdx;
		double mStartFreq;
		double mStopFreq;
		int mStepsPerOctave;
		double mIntegrationTime;
		double mTransientTime;
		double mBurstIntervalTime;
		double mSignalLevel;

		std::vector<double> mFrequencies;
		wxString mSeparator;
};

#endif // OCTAVETONEGENERATOR_H
