#include "COgg.h"

void CSound::open(char *path)
{
    int result;
    if(!(oggFile = fopen(path, "rb")))
        printf("Could not open Ogg file.");
    if((result = ov_open(oggFile, &oggStream, NULL, 0)) < 0)
    {
        fclose(oggFile);
        printf("Could not open Ogg stream. %s",errorString(result));
    }
    vorbisInfo = ov_info(&oggStream, -1);
    vorbisComment = ov_comment(&oggStream, -1);
    if(vorbisInfo->channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;
    alGenBuffers(2, buffers);
    check();
    alGenSources(1, &source);
    check();
    alSource3f(source, AL_POSITION,        0.0, 0.0, 0.0);
    alSource3f(source, AL_VELOCITY,        0.0, 0.0, 0.0);
    alSource3f(source, AL_DIRECTION,       0.0, 0.0, 0.0);
    alSourcef (source, AL_ROLLOFF_FACTOR,  0.0          );
    alSourcei (source, AL_SOURCE_RELATIVE, AL_TRUE      );
}

void CSound::release()
{
    alSourceStop(source);
    empty();
    alDeleteSources(1, &source);
    check();
    alDeleteBuffers(1, buffers);
    check();
    ov_clear(&oggStream);
    alutExit();
}

void CSound::display()
{
    printf("version: %i\nChannels: %i\nRate (Hz): %i\nBitrate: %i %i %i %i\nVendor: %s\n",
    vorbisInfo->version,vorbisInfo->channels,vorbisInfo->rate,
    vorbisInfo->bitrate_upper,vorbisInfo->bitrate_nominal,
    vorbisInfo->bitrate_lower,vorbisInfo->bitrate_window,
    vorbisComment->vendor);
    for(int i = 0; i < vorbisComment->comments; i++)
        printf("%s\n",vorbisComment->user_comments[i]);
}

bool CSound::playback()
{
    if(playing())
        return true;
    if(!stream(buffers[0]))
        return false;
    if(!stream(buffers[1]))
        return false;
    alSourceQueueBuffers(source, 2, buffers);
    alSourcePlay(source);
    return true;
}

bool CSound::playing()
{
    ALenum state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

bool CSound::update()
{
    int processed;
    bool active = true;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
    while(processed--)
    {
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
        check();
        active = stream(buffer);
        alSourceQueueBuffers(source, 1, &buffer);
        check();
    }
    return active;
}

bool CSound::stream(ALuint buffer)
{
    char pcm[BUFFER_SIZE];
    int  size = 0;
    int  section;
    int  result;
    while(size < BUFFER_SIZE)
    {
        result = ov_read(&oggStream, pcm + size, BUFFER_SIZE - size, 0, 2, 1, &section);
        if(result > 0)
            size += result;
        else
            if(result < 0)
                printf("%s\n",errorString(result));
            else
                break;
    }
    if(size == 0)
        return false;
    alBufferData(buffer, format, pcm, size, vorbisInfo->rate);
    check();    
    return true;
}

void CSound::empty()
{
    int queued;
    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
    while(queued--)
    {
        ALuint buffer;    
        alSourceUnqueueBuffers(source, 1, &buffer);
        check();
    }
}

void CSound::check()
{
	int error = alGetError();
	if(error != AL_NO_ERROR)
        printf("OpenAL error was raised.");
}

char *CSound::errorString(int code)
{
    switch(code)
    {
        case OV_EREAD:
            return (char*)"Read from media.";
        case OV_ENOTVORBIS:
            return (char*)"Not Vorbis data.";
        case OV_EVERSION:
            return (char*)"Vorbis version mismatch.";
        case OV_EBADHEADER:
            return (char*)"Invalid Vorbis header.";
        case OV_EFAULT:
            return (char*)"Internal logic fault (bug or heap/stack corruption.";
        default:
            return (char*)"Unknown Ogg error.";
    }
}
