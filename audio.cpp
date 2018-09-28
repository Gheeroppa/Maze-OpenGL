#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <cstdio>
#include <string> 

using namespace std; 

ALuint buffer, source;
ALCdevice* device;
ALCcontext* context;

void initSound()
{
  alutInit(0, NULL);
  
  device = alcOpenDevice(NULL);
  context = alcCreateContext(device, NULL);
  alcMakeContextCurrent(context);
  alGenSources(1, &source);
  
  alListener3f(AL_POSITION, 0, 0, 0);
  alListener3f(AL_VELOCITY, 0, 0, 0);

  string file = "sounds/taken.wav";
  buffer = alutCreateBufferFromFile(file.c_str());
  alSourcef(source, AL_PITCH, 1);
  alSourcef(source, AL_GAIN, 1);
  alSource3f(source, AL_POSITION, 0, 0, 0);
  alSource3f(source, AL_VELOCITY, 0, 0, 0);
  alSourcei(source, AL_LOOPING, AL_FALSE); 
  
  //attach a source to a buffer
  alSourcei(source, AL_BUFFER, buffer);
  	
}

void playSound()
{
  //play sound
  alSourcePlay(source);
  //getc(stdin);
}

void stopSound()
{
    //close source  
  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
  alcDestroyContext(context);
  alcCloseDevice(device);

  alutExit();
}
