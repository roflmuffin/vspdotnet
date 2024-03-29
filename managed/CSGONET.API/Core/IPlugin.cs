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

namespace CSGONET.API.Core
{
    /// <summary>
    /// Interface which every CSGONET plugin must implement. Module will be created with parameterless constructor and then Load method will be called.
    /// </summary>
    public interface IPlugin
    {
        /// <summary>
        /// Name of the plugin.
        /// </summary>
        string ModuleName
        {
            get;
        }
        /// <summary>
        /// Module version.
        /// </summary>
        string ModuleVersion
        {
            get;
        }
        /// <summary>
        /// This method is called by CSGONET on plugin load and should be treated as plugin constructor.
        /// </summary>
        void Load(bool hotReload);
        /// <summary>
        /// Will be called by CSGONET on plugin unload. In this method plugin must cleanup and unregister with CSGO native data.
        /// </summary>
        void Unload(bool hotReload);
    }
}