#pragma once
#include<AL/al.h>
#include<AL/alc.h>
#include<fstream>
#include<vector>
#include"../maths/vec3.h"
struct Audio{
    static ALCdevice *device;
    static ALCcontext *context;
    static void Init();
    static void Terminate();
    //static
    ALsizei sample_rate;
    ALenum format;
    short* buffer;
    int samples;
    std::string name;
    ALuint bufferID;        
    
    static Audio* findAudio(std::string tname);   
    static std::vector<Audio*> audios;
    
    Audio(unsigned char t_type,std::ifstream&);
    ~Audio();
};

struct Sound{
    vec3 position,velocity;
    bool playing,paused;
    char flags;
    float gain,pitch;
    Audio* audio;
    ALuint source;
    ALint state;
    void Play();
    void Pause(bool stop=false);
    void tick(vec3& position);
    Sound(Audio*);
    ~Sound();
    
};
#define LOOP 1
#define PLAYONAWAKE 2
#define POSITIONAL 4
