// some examples for fun
// return (t);
// return (t & t >> 8);
// return (t * (42 & t >> 10));
// return (t >> 6 | t << 1) + (t >> 5 | t << 3 | t >> 3) | t >> 2 | t << 1;
// return (t + (t & t ^ t >> 6) - t * ((t >> 9) & (t % 16 ? 2 : 6) & t >> 9));
// return ((t >> 6 | t >> 9) & 82 & t >> 9) * t;
// return (t * (t * 287 / 256 & t >> 11 & 31));
// return ((t & t + t / 256) - t * (t >> 15) & 64);
// return (t ^ t + (t >> 15 | 1) ^ (t - 1280 ^ t) >> 10) * t;
// return (t * 9 & t >> 4 | t * 5 & t >> 7 | t * 3 & t / 1024);
// return (~t >> 2) * ((127 & t * (7 & t >> 10)) < (245 & t * (2 + (5 & t >> 14))));
// return (t * 5 & t >> 7) | (t * 3 & t >> 10);
// return ((t * (t >> 8 | t >> 9) & 46 & t >> 8)) ^ (t & t >> 13 | t >> 6);
// return (t & t % 255) - (t * 3 & t >> 13 & t >> 6);
// return ((t << 1)^((t << 1)+(t >> 7) & t >> 12)) |t >> (4 - (1 ^ 7 & (t >> 19))) | t >> 7;
// -------------------------------------------------------------
// header contents
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <future>
#include <thread>
#include <iostream>
#include <conio.h> 
// required library
#pragma comment(lib, "winmm.lib")

class MakeNoise
{
public:
	void playTrack(const int& time, const int& SamplesPerSec, int loop = 1);

private:
	char* buffer;
	HWAVEOUT hWaveOut = NULL;
	WAVEFORMATEX wfx;
	// WORD  wFormatTag			Waveform-audio format type
	// WORD  nChannels			Number of channels in the waveform-audio data
	// DWORD nSamplesPerSec		Sample rate, in samples per second (hertz)
	// DWORD nAvgBytesPerSec	Average data-transfer rate - for WAVE_FORMAT_PCM, should be equal to nSamplesPerSec
	// WORD  nBlockAlign		Block alignment in bytes 
	// WORD  wBitsPerSample		Bits per sample - for WAVE_FORMAT_PCM, should be equal to 8 or 16
	// WORD  cbSize				Size, in bytes, of extra format information (ignored - 0)
};
// HERE WE CAN SET YOUR ALGORITHM (take a look at the first lines)
inline char algorithm(int t)
{
	return (t & t >> 8);
}

void MakeNoise::playTrack(const int& time, const int& samplesPerSec, int loop)
{	// allocate memory
	size_t size = static_cast<size_t>((time * samplesPerSec) / 1000);
	buffer = new char[size];
	// minimal instance
	if (loop < 1) loop = 1;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 0x01;
	wfx.nSamplesPerSec = samplesPerSec;
	wfx.nAvgBytesPerSec = samplesPerSec;
	wfx.nBlockAlign = 0x01;
	wfx.wBitsPerSample = 0x08;
	wfx.cbSize = NULL;
	// https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-waveoutopen
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, NULL, NULL, CALLBACK_NULL);
	// fills our buffer with your algorithm for track
	for (DWORD t = 0; t < size; t++)
		buffer[t] = static_cast<char>(algorithm(t));
	// header
	WAVEHDR header = { buffer, (DWORD)size, NULL, NULL, WHDR_BEGINLOOP | WHDR_ENDLOOP, (DWORD)loop, NULL, NULL };
	/*
	MMRESULT waveOutPrepareHeader/waveOutWrite/waveOutUnprepareHeader(
		HWAVEOUT  hwo,	// Handle to the waveform-audio output device
		LPWAVEHDR pwh,	// Pointer to a WAVEHDR structure that identifies the data block to be prepared
		UINT      cbwh	// Size, in bytes, of the WAVEHDR structure
	);
	*/
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR)); // clean up
	// The waveOutClose function closes the given waveform-audio output device
	waveOutClose(hWaveOut);
	/*
	MMRESULT waveOutSetVolume(
		HWAVEOUT hwo,		// Handle to the waveform-audio output device
		DWORD    dwVolume	// set volume for speakers, 4 bits for each
	);
	*/
	waveOutSetVolume(hWaveOut, 0x0FFF0FFF); // set volume
	/*
	MMRESULT waveOutSetPlaybackRate(
		HWAVEOUT hwo,		// Handle to the waveform-audio output device
		DWORD    dwRate		// New playback rate setting
	);
	*/
	waveOutSetPlaybackRate(hWaveOut, 0x0000FFFF); // pitch
	// sleep for a while - track is running
	Sleep((time * loop));
	// clean up
	delete[] buffer;
	std::cout << "The Thread Is Dead" << std::endl;
}

bool newThread()
{	// thread and argument
	std::cout << "Thread ID : " << std::this_thread::get_id() << std::endl;
	std::thread t(&MakeNoise::playTrack,	// function
		MakeNoise(),						// class
		16383,								// sequence duration (byte multiple)
		8000,								// samples per second
		INT_MAX);							// loop number 

	t.detach(); // may I should use Join instead?
	return true;
}

int main()
{
	#define ESCAPE 27
	std::cout << "Procedural Music using a single algorithm" << std::endl;
	// start our thread
	std::future<bool> bgThread = std::async(std::launch::async, newThread);
	// wait escape key to close application
	while (true)
	{
		int c = 0;

		switch ((c = _getch()))
		{
		case ESCAPE:
			return 0;
		}
	}
}