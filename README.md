andplay is a command-line audio player for Android. It was developed for
use with the <a href="http://kevinboone.net/kbox3.html">KBOX</a> 
command-line environment, but should work with plain Android as well.
<p/>
andplay is really just a way to invoke Android's built-in media player,
but from the command line. It will only play audio, but
-- and this is probably the one thing it might actually be usful for
-- it will play the audio soundtracks from video files.
<p/>
andplay should be build using an Android C compiler, such as Google's
Android NDK:
<p/>
<pre>
make CC=/path/to/android_ndk/gcc
</pre>
<p/>
To use andplay, just specify one or more files on the command line.

