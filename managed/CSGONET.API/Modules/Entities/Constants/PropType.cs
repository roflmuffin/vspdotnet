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
    /// <summary>
    /// Property types for entity
    /// </summary>
    public enum PropType
    {
        /// <summary>
        /// Property is networked
        /// </summary>
        Send,
        /// <summary>
        /// Property is a save data field
        /// </summary>
        Data
    }
}
