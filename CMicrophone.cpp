//#include "CMicrophone.h"

CMicrophone::CMicrophone() {
    data = (char*)malloc(4096);
    memset(data,0,4096);
    dev[0] = alcOpenDevice(NULL);
    if (dev[0]!=NULL) {
        ctx = alcCreateContext(dev[0], NULL);
        alcMakeContextCurrent(ctx);
        alGenSources(1, &source);
        alGenBuffers(3, buffers);
        alBufferData(buffers[0], AL_FORMAT_MONO16, data, sizeof(data), 22050);
        alBufferData(buffers[1], AL_FORMAT_MONO16, data, sizeof(data), 22050);
        alBufferData(buffers[2], AL_FORMAT_MONO16, data, sizeof(data), 22050);
        alSourceQueueBuffers(source, 3, buffers);
        alDistanceModel(AL_NONE);
        dev[1] = alcCaptureOpenDevice(NULL, 22050, AL_FORMAT_MONO16,
                                      sizeof(data)/2);
        alSourcePlay(source);
        alcCaptureStart(dev[1]);
    }
}

int CMicrophone::Capture() {
    while (1) {
        ALuint buf;
        ALint val;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
        if (val <= 0)
            continue;
        alcGetIntegerv(dev[1], ALC_CAPTURE_SAMPLES, 1, &val);
        if (val <= 0)
            continue;
        alcCaptureSamples(dev[1], data, val);
        alSourceUnqueueBuffers(source, 1, &buf);
        alBufferData(buf, AL_FORMAT_MONO16, data, val*2, 22050);
        alSourceQueueBuffers(source, 1, &buf);
        alGetSourcei(source, AL_SOURCE_STATE, &val);
        if (val != AL_PLAYING)
            alSourcePlay(source);
    }
}

CMicrophone::~CMicrophone() {
    alcCaptureStop(dev[1]);
    alcCaptureCloseDevice(dev[1]);
    alSourceStop(source);
    alDeleteSources(1, &source);
    alDeleteBuffers(3, buffers);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCloseDevice(dev[0]);
}
