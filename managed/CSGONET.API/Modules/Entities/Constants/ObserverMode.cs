namespace CSGONET.API.Modules.Entities.Constants
{
    public enum ObserverMode
    {
		OBS_MODE_NONE = 0,  // not in spectator mode
        OBS_MODE_DEATHCAM,  // special mode for death cam animation
        OBS_MODE_FREEZECAM, // zooms to a target, and freeze-frames on them
        OBS_MODE_FIXED,     // view from a fixed camera position
        OBS_MODE_IN_EYE,    // follow a player in first person view
        OBS_MODE_CHASE,     // follow a player in third person view
        OBS_MODE_ROAMING,   // free roaming

        NUM_OBSERVER_MODES,
	}
}