namespace CSGONET.API.Modules.Sound.Constants
{
    public enum SoundChannel
    {
        CHAN_REPLACE	= -1,
        CHAN_AUTO		= 0,
        CHAN_WEAPON		= 1,
        CHAN_VOICE		= 2,
        CHAN_ITEM		= 3,
        CHAN_BODY		= 4,
        CHAN_STREAM		= 5,		// allocate stream channel from the static or dynamic area
        CHAN_STATIC		= 6,		// allocate channel from the static area 
        CHAN_VOICE_BASE	= 7,		// allocate channel for network voice data
        CHAN_USER_BASE = 135
    }
}