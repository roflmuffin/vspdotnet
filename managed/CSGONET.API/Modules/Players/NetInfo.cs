using System;
using System.Reflection.Emit;
using CSGONET.API.Modules.Players.Constants;

namespace CSGONET.API.Modules.Players
{
    public class NetInfo : BaseNative
    {
        public NetInfo(IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn)
        {
        }

        protected override void OnDispose()
        {
        }

        public virtual string GetName()
        {
            string ret = NativePINVOKE.INetChannelInfo_GetName(ptr);
            return ret;
        }

        public virtual string GetAddress()
        {
            string ret = NativePINVOKE.INetChannelInfo_GetAddress(ptr);
            return ret;
        }

        public virtual float GetTime()
        {
            float ret = NativePINVOKE.INetChannelInfo_GetTime(ptr);
            return ret;
        }

        public virtual float GetTimeConnected()
        {
            float ret = NativePINVOKE.INetChannelInfo_GetTimeConnected(ptr);
            return ret;
        }

        public virtual int GetBufferSize()
        {
            int ret = NativePINVOKE.INetChannelInfo_GetBufferSize(ptr);
            return ret;
        }

        public virtual int GetDataRate()
        {
            int ret = NativePINVOKE.INetChannelInfo_GetDataRate(ptr);
            return ret;
        }

        public virtual bool IsLoopback()
        {
            bool ret = NativePINVOKE.INetChannelInfo_IsLoopback(ptr);
            return ret;
        }

        public virtual bool IsTimingOut()
        {
            bool ret = NativePINVOKE.INetChannelInfo_IsTimingOut(ptr);
            return ret;
        }

        public virtual bool IsPlayback()
        {
            bool ret = NativePINVOKE.INetChannelInfo_IsPlayback(ptr);
            return ret;
        }

        public virtual float GetLatency(NetFlow flow)
        {
            float ret = NativePINVOKE.INetChannelInfo_GetLatency(ptr, (int)flow);
            return ret;
        }

        public virtual float GetAvgLatency(NetFlow flow)
        {
            float ret = NativePINVOKE.INetChannelInfo_GetAvgLatency(ptr, (int)flow);
            return ret;
        }

        public virtual float GetAvgLoss(NetFlow flow)
        {
            float ret = NativePINVOKE.INetChannelInfo_GetAvgLoss(ptr, (int)flow);
            return ret;
        }

        public virtual float GetAvgChoke(NetFlow flow)
        {
            float ret = NativePINVOKE.INetChannelInfo_GetAvgChoke(ptr, (int)flow);
            return ret;
        }

        public virtual float GetAvgData(NetFlow flow)
        {
            float ret = NativePINVOKE.INetChannelInfo_GetAvgData(ptr, (int)flow);
            return ret;
        }

        public virtual float GetAvgPackets(NetFlow flow)
        {
            float ret = NativePINVOKE.INetChannelInfo_GetAvgPackets(ptr, (int)flow);
            return ret;
        }

        public virtual int GetTotalData(NetFlow flow)
        {
            int ret = NativePINVOKE.INetChannelInfo_GetTotalData(ptr, (int)flow);
            return ret;
        }

        public virtual int GetTotalPackets(NetFlow flow)
        {
            int ret = NativePINVOKE.INetChannelInfo_GetTotalPackets(ptr, (int)flow);
            return ret;
        }

        public virtual int GetSequenceNr(NetFlow flow)
        {
            int ret = NativePINVOKE.INetChannelInfo_GetSequenceNr(ptr, (int)flow);
            return ret;
        }

        public virtual bool IsValidPacket(NetFlow flow, int frame_number)
        {
            bool ret = NativePINVOKE.INetChannelInfo_IsValidPacket(ptr, (int) flow, frame_number);
            return ret;
        }

        public virtual float GetPacketTime(NetFlow flow, int frame_number)
        {
            float ret = NativePINVOKE.INetChannelInfo_GetPacketTime(ptr, (int)flow, frame_number);
            return ret;
        }

        public virtual int GetPacketBytes(NetFlow flow, int frame_number, int group)
        {
            int ret = NativePINVOKE.INetChannelInfo_GetPacketBytes(ptr, (int)flow, frame_number, group);
            return ret;
        }

        public virtual float GetTimeSinceLastReceived()
        {
            float ret = NativePINVOKE.INetChannelInfo_GetTimeSinceLastReceived(ptr);
            return ret;
        }

        public virtual float GetCommandInterpolationAmount(NetFlow flow, int frame_number)
        {
            float ret = NativePINVOKE.INetChannelInfo_GetCommandInterpolationAmount(ptr, (int)flow, frame_number);
            return ret;
        }

        public virtual float GetTimeoutSeconds()
        {
            float ret = NativePINVOKE.INetChannelInfo_GetTimeoutSeconds(ptr);
            return ret;
        }
    }
}