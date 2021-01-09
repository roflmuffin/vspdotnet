using System;

namespace CSGONET.API.Modules.Entities.Constants
{
    [Flags]
    public enum SolidFlags
    {
		FSOLID_CUSTOMRAYTEST		= 0x0001,	// Ignore solid type + always call into the entity for ray tests
        FSOLID_CUSTOMBOXTEST		= 0x0002,	// Ignore solid type + always call into the entity for swept box tests
        FSOLID_NOT_SOLID			= 0x0004,	// Are we currently not solid?
        FSOLID_TRIGGER				= 0x0008,	// This is something may be collideable but fires touch functions
        // even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
        FSOLID_NOT_STANDABLE		= 0x0010,	// You can't stand on this
        FSOLID_VOLUME_CONTENTS		= 0x0020,	// Contains volumetric contents (like water)
        FSOLID_FORCE_WORLD_ALIGNED	= 0x0040,	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
        FSOLID_USE_TRIGGER_BOUNDS	= 0x0080,	// Uses a special trigger bounds separate from the normal OBB
        FSOLID_ROOT_PARENT_ALIGNED	= 0x0100,	// Collisions are defined in root parent's local coordinate space
        FSOLID_TRIGGER_TOUCH_DEBRIS	= 0x0200,	// This trigger will touch debris objects
        FSOLID_TRIGGER_TOUCH_PLAYER	= 0x0400,	// This trigger will touch only players
        FSOLID_NOT_MOVEABLE			= 0x0800,	// Assume this object will not move

        FSOLID_MAX_BITS	= 12
    }
}