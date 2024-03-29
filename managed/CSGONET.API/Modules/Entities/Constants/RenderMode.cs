﻿/*
 *  This file is part of VSP.NET.
 *  VSP.NET is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VSP.NET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VSP.NET.  If not, see <https://www.gnu.org/licenses/>. *
 */

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