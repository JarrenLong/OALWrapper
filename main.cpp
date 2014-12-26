#include <stdio.h>
#include <conio.h>
#include <al/al.h>
#include <al/alc.h>
#include <al/alut.h>
#include <vector>
#define NUM_BUFFERS 6

using namespace std;

class OAL {
    public:
        void InitAL();
        void KillAL();
        void LoadWave(char *filename, int bufID);
        void CreateBuffers(int num_buffers);
        void AddSource(ALint type, ALfloat pitch, ALfloat gain);
        void SetListenerValues();
    private:
        ALCdevice* pDevice;
        ALCcontext* pContext;
        ALCubyte* defaultDevice;
        ALCcontext* pCurContext;
        ALCdevice* pCurDevice;
    	ALenum format;
        ALsizei size;
        ALvoid* data;
        ALsizei freq;
        ALboolean loop;
        ALuint Buffers[NUM_BUFFERS];
        vector<ALuint> Sources;
        ALfloat SourcePos[];// = { 0.0, 0.0, 0.0 };
        ALfloat SourceVel[];// = { 0.0, 0.0, 0.0 };
        ALfloat ListenerPos[];// = { 0.0, 0.0, 0.0 };
        ALfloat ListenerVel[];// = { 0.0, 0.0, 0.0 };
        ALfloat ListenerOri[];// = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };
};

void OAL::InitAL() {
    pDevice = alcOpenDevice(NULL);
    pContext = alcCreateContext(pDevice, NULL);
    alcMakeContextCurrent(pContext);
    alcGetError(pDevice);
    alGetError();
}

void OAL::KillAL() {
	for(vector<ALuint>::iterator iter = Sources.begin(); iter != Sources.end(); iter++)
    {alDeleteSources(1, &(*iter));}
	Sources.clear();
	alDeleteBuffers(NUM_BUFFERS, Buffers);
    pCurContext = alcGetCurrentContext();
    pCurDevice = alcGetContextsDevice(pCurContext);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(pCurContext);
    alcCloseDevice(pCurDevice);
}

void OAL::CreateBuffers(int num_buffers) {
    alGenBuffers(NUM_BUFFERS, Buffers);
}

void OAL::LoadWave(char *filename, int bufID) {
	alutLoadWAVFile(filename, &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[bufID], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
}

void OAL::AddSource(ALint type, ALfloat pitch, ALfloat gain) {
	ALuint Source;
	alGenSources(1, &Source);
	alSourcei (Source, AL_BUFFER,   Buffers[type]);
	alSourcef (Source, AL_PITCH,    pitch         );
	alSourcef (Source, AL_GAIN,     gain         );
	alSourcefv(Source, AL_POSITION, SourcePos    );
	alSourcefv(Source, AL_VELOCITY, SourceVel    );
	alSourcei (Source, AL_LOOPING,  AL_TRUE      );
	alSourcePlay(Source);
	Sources.push_back(Source);
}

void OAL::SetListenerValues() {
	alListenerfv(AL_POSITION,    ListenerPos);
	alListenerfv(AL_VELOCITY,    ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}

int main(int argc, char *argv[])
{
    OAL music;
    music.InitAL();
    music.CreateBuffers(NUM_BUFFERS);
    music.SetListenerValues();
    music.LoadWave("chimes.wav",0);
    music.LoadWave("thunder.wav",1);
    music.LoadWave("rain.wav",2);
    music.LoadWave("waterdrop.wav",3);
    music.LoadWave("ocean.wav",4);
    music.LoadWave("stream.wav",5);
    for(int i=0;i<NUM_BUFFERS;i++)
    {
        music.AddSource(i, 1.0f, 0.125f);
    }
	getche();
	music.KillAL();
	return 0;
}
