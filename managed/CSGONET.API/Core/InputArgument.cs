using System;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Security;

namespace CSGONET.API.Core
{
    public class InputArgument
    {
        protected object m_value;

        internal object Value => m_value;

        private InputArgument(object value)
        {
            m_value = value;
        }

        public override string ToString()
        {
            return m_value.ToString();
        }

        public static implicit operator InputArgument(bool value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(sbyte value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(byte value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(short value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(ushort value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(int value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(uint value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(long value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(ulong value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(float value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(double value)
        {
            return new InputArgument((float)value);
        }

        public static implicit operator InputArgument(Enum value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(string value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(Vector3 value)
        {
            return new InputArgument(value);
        }

        public static implicit operator InputArgument(Delegate value)
        {
            var functionReference = FunctionReference.Create(value);
            IntPtr cb = functionReference.GetFunctionPointer();
            return new InputArgument(cb.ToInt32());
        }

        public static implicit operator InputArgument(FunctionReference value)
        {
            IntPtr cb = value.GetFunctionPointer();
            return new InputArgument(cb.ToInt32());
        }

        [SecurityCritical]
        public static implicit operator InputArgument(IntPtr value)
        {
            return new InputArgument(value);
        }

        [SecurityCritical]
        public static unsafe implicit operator InputArgument(void* value)
        {
            return new InputArgument(new IntPtr(value));
        }
    }
}