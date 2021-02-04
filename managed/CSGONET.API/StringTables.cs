using CSGONET.API.Core;

namespace CSGONET.API
{
    public class StringTable
    {
        private int _index;

        public StringTable(int index)
        {
            _index = index;
        }

        public void Add(string value, string userdata = null)
        {
            NativeAPI.AddToStringTable(_index, value, userdata);
        }

        public int FindStringIndex(string search)
        {
            return NativeAPI.FindStringIndex(_index, search);
        }

        public string GetUserData(int stringIndex)
        {
            return NativeAPI.GetStringTableData(_index, stringIndex);
        }

        public int GetUserDataLength(int stringIndex)
        {
            return NativeAPI.GetStringTableDataLength(_index, stringIndex);
        }

        public int MaxStringNum => NativeAPI.GetStringTableMaxStrings(_index);
        public string Name => NativeAPI.GetStringTableName(_index);

        public int NumStrings => NativeAPI.GetStringTableNumStrings(_index);

        public string Read(int stringIndex) => NativeAPI.ReadStringTable(_index, stringIndex);

        public void SetUserData(int stringIndex, string value) =>
            NativeAPI.SetStringTableData(_index, stringIndex, value);

    }

    public class StringTables
    {
        private static StringTable _downloadsTable = null;
        public static StringTable Find(string search)
        {
            var index = NativeAPI.FindStringTable(search);
            if (index > -1) return new StringTable(index);

            return null;
        }

        public static int NumberOfTables => NativeAPI.GetNumStringTables();

        public static bool Lock(bool @lock) => NativeAPI.LockStringTables(@lock);

        public static void AddFileToDownloadsTable(string filename)
        {
            if (_downloadsTable == null)
            {
                _downloadsTable = Find("downloadables");
            }

            bool save = Lock(false);
            _downloadsTable.Add(filename);
            Lock(save);
        }

    }
}