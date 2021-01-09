namespace CSGONET.API.Modules.Entities.Constants
{
    public enum MoveType
    {
        MOVETYPE_NONE = 0,  // never moves
        MOVETYPE_ISOMETRIC,         // For players -- in TF2 commander view, etc.
        MOVETYPE_WALK,              // Player only - moving on the ground
        MOVETYPE_STEP,              // gravity, special edge handling -- monsters use this
        MOVETYPE_FLY,               // No gravity, but still collides with stuff
        MOVETYPE_FLYGRAVITY,        // flies through the air + is affected by gravity
        MOVETYPE_VPHYSICS,          // uses VPHYSICS for simulation
        MOVETYPE_PUSH,              // no clip to world, push and crush
        MOVETYPE_NOCLIP,            // No gravity, no collisions, still do velocity/avelocity
        MOVETYPE_LADDER,            // Used by players only when going onto a ladder
        MOVETYPE_OBSERVER,          // Observer movement, depends on player's observer mode
        MOVETYPE_CUSTOM,            // Allows the entity to describe its own physics

        // should always be defined as the last item in the list
        MOVETYPE_LAST = MOVETYPE_CUSTOM,

        MOVETYPE_MAX_BITS = 4
    }
}