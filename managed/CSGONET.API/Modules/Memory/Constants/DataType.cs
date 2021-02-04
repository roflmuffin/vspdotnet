using System;
using System.Collections.Generic;

namespace CSGONET.API.Modules.Memory.Constants
{
    public enum DataType
    {
		DATA_TYPE_VOID,
        DATA_TYPE_BOOL,
        DATA_TYPE_CHAR,
        DATA_TYPE_UCHAR,
        DATA_TYPE_SHORT,
        DATA_TYPE_USHORT,
        DATA_TYPE_INT,
        DATA_TYPE_UINT,
        DATA_TYPE_LONG,
        DATA_TYPE_ULONG,
        DATA_TYPE_LONG_LONG,
        DATA_TYPE_ULONG_LONG,
        DATA_TYPE_FLOAT,
        DATA_TYPE_DOUBLE,
        DATA_TYPE_POINTER,
        DATA_TYPE_STRING,
        DATA_TYPE_VARIANT
    }

    public static class DataTypeExtensions
    {
        private static Dictionary<Type, DataType> types = new Dictionary<Type, DataType>()
        {
            {typeof(float), DataType.DATA_TYPE_FLOAT},
            {typeof(IntPtr), DataType.DATA_TYPE_POINTER},
            {typeof(int), DataType.DATA_TYPE_INT},
            {typeof(bool), DataType.DATA_TYPE_BOOL},
            {typeof(string), DataType.DATA_TYPE_STRING},
            {typeof(ulong), DataType.DATA_TYPE_ULONG },
            {typeof(short), DataType.DATA_TYPE_VARIANT }
        };

        public static DataType? ToDataType(this Type type)
        {
            if (types.ContainsKey(type)) return types[type];

            if (typeof(NativeObject).IsAssignableFrom(type))
            {
                return DataType.DATA_TYPE_POINTER;
            }

            return null;
        }
    }
}