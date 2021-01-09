namespace CSGONET.API.Modules.Entities.Constants
{
	public enum RenderMode
    {
        RENDER_NORMAL,              /**< src */
        RENDER_TRANSCOLOR,          /**< c*a+dest*(1-a) */
        RENDER_TRANSTEXTURE,        /**< src*a+dest*(1-a) */
        RENDER_GLOW,                /**< src*a+dest -- No Z buffer checks -- Fixed size in screen space */
        RENDER_TRANSALPHA,          /**< src*srca+dest*(1-srca) */
        RENDER_TRANSADD,            /**< src*a+dest */
        RENDER_ENVIRONMENTAL,       /**< not drawn, used for environmental effects */
        RENDER_TRANSADDFRAMEBLEND,  /**< use a fractional frame value to blend between animation frames */
        RENDER_TRANSALPHAADD,       /**< src + dest*(1-a) */
        RENDER_WORLDGLOW,           /**< Same as kRenderGlow but not fixed size in screen space */
        RENDER_NONE                 /**< Don't render. */
    };
}