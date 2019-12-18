#include"audio.h"
#include"stb_vorbis.c"
std::vector<Audio*> Audio::audios;
ALCcontext* Audio::context=0;
ALCdevice* Audio::device=0;

Audio::Audio(unsigned char t_type, std::ifstream& ifs)
{
    
//     ifs.read((char*)&format,4);        // format 
//     ifs.read((char*)&freq,4);          // frequency

    int len=0;
    unsigned char *mem=0;
    
    ifs.read((char*)&len,4);          //buffer size
    mem=new unsigned char[len];
    ifs.read((char*)mem,len);         //buffer itself
    
    int channels;
    samples = stb_vorbis_decode_memory(mem, len, &channels, &sample_rate, &buffer);
    delete []mem;
    if (channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;

    alGenBuffers((ALuint)1, &bufferID);
    alBufferData(bufferID, format, buffer, samples*channels*sizeof(short), sample_rate);
     
    audios.push_back(this);
    printf("size of buffer= %d freq=%d\n",len,sample_rate);
}

void Audio::Init(){
  device= alcOpenDevice(NULL);  
  
  if(!device){printf("no device!!!\n");}
  else{
    context=alcCreateContext(device,NULL);
    alcMakeContextCurrent(context);
  }
    
}
void Audio::Terminate()
{
    alcCloseDevice(device);
}

Audio::~Audio()
{
    free(buffer);
     alDeleteBuffers(1, &bufferID);
    
}
Audio* Audio::findAudio(std::string tname){
    Audio* t=0;
    for(int i=0;i<audios.size();i++){
        if(tname==audios[i]->name ){t=audios[i];break;}
    }
    return t;
}
/// /////////////////////////

Sound::Sound(Audio* audio)
{
    this->audio=audio;
    alGenSources((ALuint)1, &source);
    alSourcei(source, AL_BUFFER, audio->bufferID);
    playing=paused=flags=0;
    pitch=gain=1.0f;
}
void Sound::tick(vec3& pos){
    position=pos;
    alSourcef(source, AL_PITCH, pitch);
    // check for errors
    alSourcef(source, AL_GAIN, gain);
    // check for errors
    // check for errors
    alSourcei(source, AL_LOOPING,bool(flags&LOOP));
    
//     alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z);
    // check for errors
//     alSource3f(source, AL_VELOCITY, 0, 0, 0);

    alGetSourcei(source, AL_SOURCE_STATE, &state);
}

void Sound::Play(){
    alSourcePlay(source);
    playing=true;paused=false;printf("------------- play Sound \n\n\n");
}
void Sound::Pause(bool stop){
     if(stop)
         alSourceStop(source);
     else
         alSourcePause(source);
     playing=false;paused=true;
}
Sound::~Sound()
{
    alSourceStop(source);
    alDeleteSources(1, &source);
}

