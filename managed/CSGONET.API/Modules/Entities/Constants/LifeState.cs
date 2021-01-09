namespace CSGONET.API.Modules.Entities.Constants
{
    public enum LifeState
    {
         LIFE_ALIVE = 0, // alive
         LIFE_DYING,				 // playing death animation or still falling off of a ledge waiting to hit ground
         LIFE_DEAD,				 // dead. lying still.
         LIFE_RESPAWNABLE,		
         LIFE_DISCARDBODY,		
    }
}