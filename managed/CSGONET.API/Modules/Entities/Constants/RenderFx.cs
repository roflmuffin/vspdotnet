namespace CSGONET.API.Modules.Entities.Constants
{
	public enum RenderFx
    {
        RENDERFX_NONE = 0,
        RENDERFX_PULSE_SLOW,
        RENDERFX_PULSE_FAST,
        RENDERFX_PULSE_SLOW_WIDE,
        RENDERFX_PULSE_FAST_WIDE,
        RENDERFX_FADE_SLOW,
        RENDERFX_FADE_FAST,
        RENDERFX_SOLID_SLOW,
        RENDERFX_SOLID_FAST,
        RENDERFX_STROBE_SLOW,
        RENDERFX_STROBE_FAST,
        RENDERFX_STROBE_FASTER,
        RENDERFX_FLICKER_SLOW,
        RENDERFX_FLICKER_FAST,
        RENDERFX_NO_DISSIPATION,
        RENDERFX_DISTORT,           /**< Distort/scale/translate flicker */
        RENDERFX_HOLOGRAM,          /**< kRenderFxDistort + distance fade */
        RENDERFX_EXPLODE,           /**< Scale up really big! */
        RENDERFX_GLOWSHELL,         /**< Glowing Shell */
        RENDERFX_CLAMP_MIN_SCALE,   /**< Keep this sprite from getting very small (SPRITES only!) */
        RENDERFX_ENV_RAIN,          /**< for environmental rendermode, make rain */
        RENDERFX_ENV_SNOW,          /**<  "        "            "    , make snow */
        RENDERFX_SPOTLIGHT,         /**< TEST CODE for experimental spotlight */
        RENDERFX_RAGDOLL,           /**< HACKHACK: TEST CODE for signalling death of a ragdoll character */
        RENDERFX_PULSE_FAST_WIDER,
        RENDERFX_MAX
    };
}