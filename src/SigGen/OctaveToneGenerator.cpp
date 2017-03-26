#include "OctaveToneGenerator.h"

#define T_PI 3.1415926535897932384626433832795028841971693993751

OctaveToneGenerator::OctaveToneGenerator(double sampleRate, int channels)
:mParamsNode(NULL)
,mSampleRate(sampleRate)
,mNoChannels(channels)
{
    //ctor
	mSeparator = wxT("\\");
}

OctaveToneGenerator::~OctaveToneGenerator()
{
    //dtor
}

bool 
OctaveToneGenerator::generateSignal(wxXmlNode* parameters)
{
	bool bRes = false;
	
	setParameters(parameters);

	generateFrequenciesList();

	bRes = writeTestFile();

	return bRes;
}

void
OctaveToneGenerator::setParameters( wxXmlNode* paramsNode )
{
    mParamsNode = paramsNode;

	//set default parameter values
	mStartFreq = 100;
	mStopFreq = mSampleRate/2;
	mStepsPerOctave = 6;
	mIntegrationTime = 500;
	mTransientTime = 250;
	mBurstIntervalTime = 250;
	mSignalLevel = 0;

	//get parameters from xml node
	wxXmlNode* parameterNode = mParamsNode->GetChildren();
	while (parameterNode)
	{
		wxString pName = parameterNode->GetAttribute(wxT("name"));

		if (pName == wxT("freqstart"))
		{
			wxString value = parameterNode->GetAttribute(wxT("value"));
			value.ToDouble(&mStartFreq);
		}
		else if (pName == wxT("freqstop"))
		{
			wxString value = parameterNode->GetAttribute(wxT("value"));
			value.ToDouble(&mStopFreq);
		}
		else if (pName == wxT("octsteps"))
		{
			wxString value = parameterNode->GetAttribute(wxT("value"));
			double bdm; value.ToDouble(&bdm);
			mStepsPerOctave = (int)bdm;
		}
		else if (pName == wxT("inttime"))
		{
			wxString value = parameterNode->GetAttribute(wxT("value"));
			value.ToDouble(&mIntegrationTime);
		}
		else if (pName == wxT("transtime"))
		{
			wxString value = parameterNode->GetAttribute(wxT("value"));
			value.ToDouble(&mTransientTime);
		}
		else if (pName == wxT("bursttime"))
		{
			wxString value = parameterNode->GetAttribute(wxT("value"));
			value.ToDouble(&mBurstIntervalTime);
		}
		else if (pName == wxT("level"))
		{
			wxString value = parameterNode->GetAttribute(wxT("value"));
			value.ToDouble(&mSignalLevel);
		}
		else if (pName == wxT("workfolder"))
		{
			mFolderPath = parameterNode->GetAttribute(wxT("value"));
		}
		else if (pName == wxT("testfile"))
		{
			mFileName = parameterNode->GetAttribute(wxT("value"));
		}

		parameterNode = parameterNode->GetNext();
	}

	int uu = 0;
}

void 
OctaveToneGenerator::generateFrequenciesList()
{
	mFrequencies.clear();
	double baseFreq = 1000;
	int lowOct = -6* mStepsPerOctave;
	int hiOct = 6* mStepsPerOctave;

	for (int i = lowOct; i < hiOct; i++)
	{
		double fc = baseFreq*(pow(2, (double)i / mStepsPerOctave));
		if ((fc >= mStartFreq) && (fc <= mStopFreq + 500))
		{
			fc = ceil(fc);

			if (fc > 1e3)
			{
				fc = 10 * ceil(fc / 10) + 4;
			}

			if (fc > 10e3)
			{
				fc = 100 * ceil(fc / 100) + 44;
			}

			mFrequencies.push_back(fc);
			fprintf(stderr, "%g\t", fc);
		}
	}
	wxMilliSleep(2000);
}

bool 
OctaveToneGenerator::writeTestFile()
{
	bool bRes = false;
	size_t sampleSilence = 1e-3 * mBurstIntervalTime * mSampleRate;
	size_t sampleTransient = 1e-3 * mTransientTime * mSampleRate;
	size_t sampleTone = 1e-3 * mIntegrationTime * mSampleRate;
	size_t writeLength = 1024;
	
	float* silenceBuffer = new float[writeLength * (size_t)mNoChannels];
	memset(silenceBuffer, 0, sizeof(float)*writeLength*mNoChannels);

	float* toneBuffer = new float[writeLength * (size_t)mNoChannels];


	wxString filePath = mFolderPath + mSeparator + mFileName;
	
	std::string strpath(filePath.mbc_str());
	WavFileWriter* writer = new WavFileWriter(strpath, (size_t)mNoChannels, (size_t)mSampleRate, 1);

	if (writer && writer->isOK())
	{
		size_t noTones = mFrequencies.size();
		for (size_t fIdx = 0; fIdx < noTones; fIdx++)
		{
			//write silence between tones;
			size_t sCount = 0;
			while (sCount < sampleSilence)
			{
				writer->writeAudioFrames(silenceBuffer, writeLength);
				sCount += writeLength;
			}
			
			//write tone burst
			size_t totalTSamples = 2 * mTransientTime + sampleTone;
			double freq = mFrequencies[fIdx];
			double linLevel = pow(10, (mSignalLevel / 20.0));
			double twoPi = T_PI * 2;
			double dTime = 1.0 / mSampleRate;
			double timeS = dTime;
			double angleS = 0;
			
			float* aFrame = new float[2];
			size_t tCount = 0;
			while (tCount < totalTSamples)
			{
				for (size_t i = 0; i < writeLength; i++)
				{
					angleS = freq*timeS;
					//if (angleS == 1.0)
						//angleS = 0.0;

					double sig = (linLevel*sin(twoPi*angleS));

					for (int j = 0; j < mNoChannels; j++)
					{
						toneBuffer[mNoChannels* i + j] = (float)sig;
					}
					timeS = (double)tCount / mSampleRate;// += dTime;
					tCount++;
				}
				writer->writeAudioFrames(toneBuffer, writeLength);
				//tCount += writeLength;
			}

			delete[] aFrame;
		}

		//write lead-out silence;
		size_t sCount = 0;
		while (sCount < sampleSilence)
		{
			writer->writeAudioFrames(silenceBuffer, writeLength);
			sCount += writeLength;
		}

		delete writer;
		bRes = true;
	}

	delete[] toneBuffer;
	delete[] silenceBuffer;

	return bRes;
}