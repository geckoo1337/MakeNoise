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
#include <thread>
#include <conio.h>
#include <cmath>

#pragma comment(lib, "winmm.lib")
// ============================================================
// CORE CLASS
// ============================================================
class MakeNoise
{
public:
    void playTrack(const int& time, const int& SamplesPerSec, int loop = 1);

private:
    unsigned char* buffer = nullptr;
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX wfx;
};
// ============================================================
// HERE WE CAN SET YOUR ALGORITHM
// ============================================================
inline unsigned char algorithm(int t)
{
    int v = (t & (t >> 8));
    v = v & 255;
    return static_cast<unsigned char>(v);
}
// ============================================================
// AUDIO ENGINE
// ============================================================
void MakeNoise::playTrack(const int& time, const int& samplesPerSec, int loop)
{
    // allocate memory
    size_t size = static_cast<size_t>((time * samplesPerSec) / 1000);
    buffer = new unsigned char[size];
    // minimal instance
    if (loop < 1) loop = 1;
    // =========================================================
    // WAV header (FIX IMPORTANT)
    // =========================================================
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = samplesPerSec;
    wfx.nAvgBytesPerSec = samplesPerSec;
    wfx.nBlockAlign = 1;
    wfx.wBitsPerSample = 8;
    wfx.cbSize = 0;

    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    // fills our buffer with your algorithm for track
    for (DWORD t = 0; t < size; t++)
        buffer[t] = algorithm(t);
    // =========================================================
    // header
    // =========================================================
    WAVEHDR header{};
    header.lpData = (LPSTR)buffer;
    header.dwBufferLength = (DWORD)size;
    header.dwFlags = 0;

    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
   
    while (!(header.dwFlags & WHDR_DONE))
        Sleep(1);

    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);

    delete[] buffer;

    std::cout << "The Thread Is Dead" << std::endl;
}
// ============================================================
// THREAD WRAPPER (FIX SAFE USAGE)
// ============================================================
bool newThread()
{
    std::cout << "Thread ID : " << std::this_thread::get_id() << std::endl;

    std::thread t([]()
    {
        MakeNoise m;
        m.playTrack(16383, 8000, 1);
    });

    t.detach();
    return true;
}
// ============================================================
// MAIN
// ============================================================
int main()
{
    #define ESCAPE 27
    std::cout << "Procedural Music using a single algorithm" << std::endl;

    std::future<bool> bgThread = std::async(std::launch::async, newThread);

    while (true)
    {
        if (_kbhit())
        {
            int c = _getch();
            if (c == ESCAPE)
                return 0;
        }
    }
}
