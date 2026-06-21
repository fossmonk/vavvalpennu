#include <raylib.h>
#include <vpconfig.h>
#include <audio.h>

void audio_init(void) {
    InitAudioDevice();
    while(!IsAudioDeviceReady());
}

void audio_deinit(void) {
    CloseAudioDevice();
}