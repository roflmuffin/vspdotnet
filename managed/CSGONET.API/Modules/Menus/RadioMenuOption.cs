using System;

namespace CSGONET.API.Modules.Menus
{
    public interface IRadioMenuOption
    {
        string Text { get; set; }
        string Value { get; set; }
        bool Disabled { get; set; }
        IntPtr Handle { get; }
    }

    public class RadioMenuOption : NativeObject, IRadioMenuOption
    {
        public RadioMenuOption(IntPtr handle) : base(handle)
        {
        }

        public RadioMenuOption(string text, bool disabled, string value = null) : base(IntPtr.Zero)
        {
            Text = text;
            Disabled = disabled;
            Value = value;
        }

        public string Text { get; set; }
        public bool Disabled { get; set; }
        public string Value { get; set; }
    }
}