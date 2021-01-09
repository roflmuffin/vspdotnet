using System;

namespace CSGONET.API.Modules.Sound.Constants
{
    [Flags]
    public enum SoundFlags
    {
        SND_NOFLAGS			= 0,			// to keep the compiler happy
        SND_CHANGE_VOL		= (1<<0),		// change sound vol
        SND_CHANGE_PITCH	= (1<<1),		// change sound pitch
        SND_STOP			= (1<<2),		// stop the sound
        SND_SPAWNING		= (1<<3),		// we're spawning, used in some cases for ambients
        // not sent over net, only a param between dll and server.
        SND_DELAY			= (1<<4),		// sound has an initial delay
        SND_STOP_LOOPING	= (1<<5),		// stop all looping sounds on the entity.
        SND_SPEAKER			= (1<<6),		// being played again by a microphone through a speaker
 
        SND_SHOULDPAUSE		= (1<<7),		// this sound should be paused if the game is paused
        SND_IGNORE_PHONEMES	= (1<<8),
        SND_IGNORE_NAME		= (1<<9),		// used to change all sounds emitted by an entity, regardless of scriptname
        SND_IS_SCRIPTHANDLE			= (1<<10),		// server has passed the actual SoundEntry instead of wave filename

        SND_UPDATE_DELAY_FOR_CHOREO	= (1<<11),		// True if we have to update snd_delay_for_choreo with the IO latency.
        SND_GENERATE_GUID			= (1<<12),	
    }
}