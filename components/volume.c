/* See LICENSE file for copyright and license details. */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../slstatus.h"
#include "../util.h"

#include <alsa/asoundlib.h>

static const char *devname = "default";
const char *
vol_perc(const char *mixname)
{
	snd_mixer_t *mixer = NULL;
	snd_mixer_selem_id_t *mixid = NULL;
	snd_mixer_elem_t *elem = NULL;
	long min = 0, max = 0, volume = -1;
	int err, muted = 0;
	if ((err = snd_mixer_open(&mixer, 0)))
	{
		warn("snd_mixer_open: %d", err);
		return NULL;
	}
	if ((err = snd_mixer_attach(mixer, devname)))
	{
		warn("snd_mixer_attach(mixer, \"%s\"): %d", devname, err);
		goto cleanup;
	}
	if ((err = snd_mixer_selem_register(mixer, NULL, NULL)))
	{
		warn("snd_mixer_selem_register(mixer, NULL, NULL): %d", err);
		goto cleanup;
	}
	if ((err = snd_mixer_load(mixer)))
	{
		warn("snd_mixer_load(mixer): %d", err);
		goto cleanup;
	}
	snd_mixer_selem_id_alloca(&mixid);
	snd_mixer_selem_id_set_name(mixid, mixname);
	snd_mixer_selem_id_set_index(mixid, 0);
	elem = snd_mixer_find_selem(mixer, mixid);
	if (!elem)
	{
		warn("snd_mixer_find_selem(mixer, \"%s\") == NULL", mixname);
		goto cleanup;
	}
	if ((err = snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_MONO, &muted)))
	{
		warn("snd_mixer_selem_get_playback_switch(): %d", err);
		goto cleanup;
	}
	if ((err = snd_mixer_selem_get_playback_volume_range(elem, &min, &max)))
	{
		warn("snd_mixer_selem_get_playback_volume_range(): %d", err);
		goto cleanup;
	}
	if ((err = snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &volume)))
	{
		warn("snd_mixer_selem_get_playback_volume(): %d", err);
	}
cleanup:
	snd_mixer_free(mixer);
	snd_mixer_detach(mixer, devname);
	snd_mixer_close(mixer);
	return muted == 0 ? "-1" : volume == -1 ? NULL
																					: bprintf("%.0f", (volume - min) * 100. / (max - min));
}
