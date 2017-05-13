/*
 * andplay -- a simple command-line audio player for Android platform
 */

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


SLEngineItf engineEngine = NULL;
SLObjectItf outputMixObject = NULL;
SLObjectItf engineObject = NULL;

// Horrible kludge for Chromebooks -- some Chrome library ties to 
//  call this function, even though (presumably) no cryptography
//  is actually done. So somebody needs to provide the symbol. 
void CRYPTO_library_init(void){}

void play (const char *uri, char **error)
  {
  SLPlayItf uriPlayerPlay = NULL;
  SLObjectItf uriPlayerObject = NULL;
  SLresult result;

  SLDataLocator_URI loc_uri = {SL_DATALOCATOR_URI, (SLchar *) uri};
  SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, 
    SL_CONTAINERTYPE_UNSPECIFIED};
  SLDataSource audioSrc = {&loc_uri, &format_mime};

  SLDataLocator_OutputMix loc_outmix = 
    {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
  SLDataSink audioSnk = {&loc_outmix, NULL};

  result = (*engineEngine)->CreateAudioPlayer
     (engineEngine, &uriPlayerObject, &audioSrc,
         &audioSnk, 0, NULL, NULL);
  assert(SL_RESULT_SUCCESS == result);

  result = (*uriPlayerObject)->Realize(uriPlayerObject, SL_BOOLEAN_FALSE);
  assert(SL_RESULT_SUCCESS == result);
   
  result = (*uriPlayerObject)->GetInterface(uriPlayerObject, 
     SL_IID_PLAY, &uriPlayerPlay);
  assert(SL_RESULT_SUCCESS == result);

  if (NULL != uriPlayerPlay) 
     {
     result = (*uriPlayerPlay)->SetPlayState(uriPlayerPlay,  
       SL_PLAYSTATE_PLAYING);
      assert(SL_RESULT_SUCCESS == result);
     }

  BOOL playing = TRUE;
  do  
    {
    usleep (500000);
    SLuint32 pState;
    (*uriPlayerPlay)->GetPlayState(uriPlayerPlay,  
       &pState);
    if (pState != SL_PLAYSTATE_PLAYING) playing = FALSE;
    } while (playing);

  if (uriPlayerObject != NULL) 
    {
    (*uriPlayerObject)->Destroy(uriPlayerObject);
    }
  }


void init (char **error)
  {
  SLresult result;
  SLEngineOption engineOptions[] = {{SL_ENGINEOPTION_THREADSAFE,
    SL_BOOLEAN_TRUE}};
  result = slCreateEngine(&engineObject, 1, engineOptions, 0, NULL, NULL);
  if (result != SL_RESULT_SUCCESS)
    {
    *error = strdup ("Can't create SLES engine");
    return;
    }

  result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
  if (result != SL_RESULT_SUCCESS)
    {
    *error = strdup ("Can't realize SLES engine");
    return;
    }

  result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, 
    &engineEngine);
  if (result != SL_RESULT_SUCCESS)
    {
    *error = strdup ("Can't get engine interface");
    return;
    }

  const SLInterfaceID ids[1] = {SL_IID_VOLUME};
  const SLboolean req[1] = {SL_BOOLEAN_FALSE};
  result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 
    1, ids, req);
  if (result != SL_RESULT_SUCCESS)
    {
    *error = strdup ("Can't create output mix interface");
    return;
    }

  result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
  if (result != SL_RESULT_SUCCESS)
    {
    *error = strdup ("Can't realize output mix object");
    return;
    }

  }


void uninit (char **error)
  {
  if (engineObject != NULL) 
    {
    (*engineObject)->Destroy(engineObject);
    }

  if (outputMixObject != NULL) 
    {
    // This call segfaults if nothing has been played :/
    //(*outputMixObject)->Destroy(outputMixObject);
    }
  }


void showUsage (void)
  {
  printf ("Usage: andplay -v -h {files...}\n");
  }


int main (int argc, char **argv)
  {
  int c;
  BOOL version = FALSE, help = FALSE;

  while ((c = getopt (argc, argv, "vh")) != -1)
    {
    switch (c)
      {
      case 'v': 
        version = TRUE; break;
      case 'h': 
        help = TRUE; break;
      }
    }

  if (version)
    {
    printf ("andplay " VERSION "\n");
    printf ("(c)2011-2017 Kevin Boone.\n");
    printf ("Distributed under the terms of the GPL v3.0.\n");
    exit (0);
    }

  if (help)
    {
    showUsage();
    exit (0);
    }

  char *error = NULL;
  init (&error);
  if (error)
    {
    fprintf (stderr, "%s\n", error);
    free (error);
    exit (-1);
    }

  int i;
  BOOL doneSomething = FALSE;
  for (i = optind; i < argc; i++)
    {
    error = NULL;
    play (argv[i], &error);
    doneSomething = TRUE;
    if (error)
      {
      fprintf (stderr, "%s\n", error);
      free (error);
      }
    } 

  if (!doneSomething)
    showUsage();

  uninit (&error);
  if (error)
    {
    // Not much point reporting it here
    free (error);
    exit (-1);
    }

  return 0;
  }

