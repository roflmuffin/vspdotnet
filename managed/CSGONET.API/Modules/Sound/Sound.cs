using System;
using System.Runtime.InteropServices;
using CSGONET.API.Core;
using CSGONET.API.Modules.Sound.Constants;
using CSGONET.API.Modules.Utils;

namespace CSGONET.API.Modules.Sound
{
    public static class Sound
    {
        public static bool PrecacheSound(string sound) => NativeAPI.PrecacheSound(sound, true);

        public static bool IsSoundPrecached(string sound) => NativeAPI.IsSoundPrecached(sound);

        public static float GetSoundDuration(string sound) => NativeAPI.GetSoundDuration(sound);

        public static void EmitSound(int client,
            string sound, 
            int entity = SoundSource.SOUND_FROM_PLAYER,
            SoundChannel channel = SoundChannel.CHAN_AUTO,
            SoundLevel level = SoundLevel.SNDLVL_NORM,
            float attenuation = SoundAttenuation.ATTN_NORM,
            SoundFlags flags = SoundFlags.SND_NOFLAGS,
            float volume = 1.0f,
            int pitch = SoundPitch.PITCH_NORM,
            Vector origin = null,
            Vector direction = null)
        {
            NativeAPI.EmitSound(client, entity, (int) channel, sound, volume, attenuation, (int) flags,
                pitch, origin?.Handle ?? IntPtr.Zero, direction?.Handle ?? IntPtr.Zero);
        }
    }
}