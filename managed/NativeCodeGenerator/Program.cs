using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using HandlebarsDotNet;
using Newtonsoft.Json;

namespace NativeCodeGenerator
{

    class NativeDefinition
    {
        [JsonProperty("name")]
        public string Name { get; set; }

        public string NameCamelCase => Regex.Replace(Name.ToLower(), "(?:^|_| +)(.)", match => match.Groups[1].Value.ToUpper());

        [JsonProperty("args")]
        public Dictionary<string, string> Arguments { get; set; }

        [JsonProperty("returns")]
        public string ReturnType { get; set; }
    }

    class Program
    {
        public static TextInfo USTextInfo = new CultureInfo("en-US", false).TextInfo;

        private static string GetPushType(string argType)
        {
            switch (argType)
            {
                case "int":
                case "uint":
                case "float":
                case "IntPtr":
                case "bool":
                case "double":
                    return "Push(";
                case "func":
                    return "Push((InputArgument)";
                case "charPtr":
                    return "PushString(";
            }

            return "NOTSUPPORTED";

        }

        private static string GetCSharpType(string type)
        {
            switch (type)
            {
                case "int":
                    return "int";
                case "uint":
                    return "uint";
                case "bool":
                    return "bool";
                case "IntPtr":
                    return "IntPtr";
                case "charPtr":
                    return "string";
                case "float":
                    return "float";
                case "double":
                    return "double";
                case "void":
                    return "void";
                case "func":
                    return "InputArgument";
            }

            return "NOTSUPPORTED";
        }

        private static ulong GetHash(string name)
        {
            uint result = 5381;

            for (int i = 0; i < name.Length; i++)
            {
                result = ((result << 5) + result) ^ name[i];
            }

            return result;
        }

        static void Main(string[] args)
        {
            var pathToSearch = @"C:\Users\Michael\source\repos\cmake_test\";

            var natives = new List<NativeDefinition>();
            foreach (string file in Directory.EnumerateFiles(pathToSearch, "natives*json", SearchOption.AllDirectories))
            {
                var data = File.ReadAllText(file);
                var localNatives = JsonConvert.DeserializeObject<NativeDefinition[]>(data);
                natives.AddRange(localNatives);
            }

            var nativesString = string.Join("\n", natives.Select(native =>
            {
                if (native.Arguments == null) native.Arguments = new Dictionary<string, string>();
                var arguments = string.Join(", ", native.Arguments.Select((x, i) => $"{GetCSharpType(x.Value)} {x.Key}"));

                var returnStr = new StringBuilder($@"
        public static {GetCSharpType(native.ReturnType ?? "void")} {native.NameCamelCase}({arguments}){{{Environment.NewLine}");

                returnStr.Append("\t\t\tlock (ScriptContext.GlobalScriptContext.Lock) {\n");
                returnStr.Append("\t\t\tScriptContext.GlobalScriptContext.Reset();\n");
                foreach (var kv in native.Arguments)
                {
                    returnStr.Append($"\t\t\tScriptContext.GlobalScriptContext.{GetPushType(kv.Value)}{kv.Key});\n");
                }

                returnStr.Append($"\t\t\tScriptContext.GlobalScriptContext.SetIdentifier({string.Format("0x{0:X}", GetHash(native.Name))});\n");
                returnStr.Append("\t\t\tScriptContext.GlobalScriptContext.Invoke();\n");
                returnStr.Append("\t\t\tScriptContext.GlobalScriptContext.CheckErrors();\n");

                if (native.ReturnType != null)
                {
                    returnStr.Append($"\t\t\treturn ({GetCSharpType(native.ReturnType)})ScriptContext.GlobalScriptContext.GetResult(typeof({GetCSharpType(native.ReturnType)}));\n");
                }

                returnStr.Append("\t\t\t}\n");
                returnStr.Append("\t\t}");

                return returnStr.ToString();
            }));

            var result = $@"
using System;

namespace CSGONET.API.Core
{{
    public class NativeAPI {{
        {nativesString}
    }}
}}
";

            Console.Write(result);

            File.WriteAllText(@"C:\Users\Michael\source\repos\cmake_test\managed\CSGONET.API\Core\API.cs", result);
        }
    }
}
