#include "gorilla/ga.h"
#include "gorilla/gau.h"

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv)
{
  ga_Format fmt;
  ga_Device* dev;
  ga_int16* buf;
  ga_int32 numSamples;
  ga_int32 sampleSize;
  ga_int32 numToQueue;
  ga_Mixer* mixer;
  ga_Handle* handle;

  /* Initialize library */
  ga_initialize(0);

  /* Initialize device */
  dev = ga_device_open(GA_DEVICE_TYPE_OPENAL);
  if(!dev)
    return 1;

  /* Initialize mixer */
  memset(&fmt, 0, sizeof(ga_Format));
  fmt.bitsPerSample = 16;
  fmt.numChannels = 2;
  fmt.sampleRate = 44100;
  numSamples = 2048;
  sampleSize = ga_format_sampleSize(&fmt);
  buf = (ga_int16*)malloc(numSamples * sampleSize);
  mixer = ga_mixer_create(&fmt, numSamples);

  /* Create and play streaming sound */
  handle = gau_stream_file(mixer, 0, 131072, "test.wav", GA_FILE_FORMAT_WAV, 0);
  ga_handle_setParami(handle, GA_HANDLE_PARAM_LOOP, 1);
  ga_handle_play(handle);

  /* Infinite mix/queue/dispatch loop */
  while(1)
  {
    numToQueue = ga_device_check(dev);
    while(numToQueue--)
    {
      ga_mixer_stream(mixer);
      ga_mixer_mix(mixer, buf);
      ga_mixer_dispatch(mixer);
      ga_device_queue(dev, &fmt, numSamples, (char*)buf);
    }
    printf("%d / %d\n", ga_handle_tell(handle, GA_TELL_PARAM_CURRENT), ga_handle_tell(handle, GA_TELL_PARAM_TOTAL));
  }

  /* Clean up mixer/device/library */
  free(buf);
  ga_handle_destroy(handle);
  ga_mixer_destroy(mixer);
  ga_device_close(dev);
  ga_shutdown();

  return 0;
}
