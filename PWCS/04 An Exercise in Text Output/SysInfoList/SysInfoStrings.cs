//---------------------------------------------
// SysInfoStrings.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoStrings
{
     public static string[] Labels
     {
          get
          {
               return new string[] 
               {
                    "ArrangeDirection",
                    "ArrangeStartingPosition",
                    "BootMode",
                    "Border3DSize",
                    "BorderSize",
                    "CaptionButtonSize",
                    "CaptionHeight",
                    "ComputerName",
                    "CursorSize",
                    "DbcsEnabled",
                    "DebugOS",
                    "DoubleClickSize",
                    "DoubleClickTime",
                    "DragFullWindows",
                    "DragSize",
                    "FixedFrameBorderSize",
                    "FrameBorderSize",
                    "HighContrast",
                    "HorizontalScrollBarArrowWidth",
                    "HorizontalScrollBarHeight",
                    "HorizontalScrollBarThumbWidth",
                    "IconSize",
                    "IconSpacingSize",
                    "KanjiWindowHeight",
                    "MaxWindowTrackSize",
                    "MenuButtonSize",
                    "MenuCheckSize",
				    "MenuFont",
                    "MenuHeight",
                    "MidEastEnabled",
                    "MinimizedWindowSize",
                    "MinimizedWindowSpacingSize",
                    "MinimumWindowSize",
                    "MinWindowTrackSize",
                    "MonitorCount",
                    "MonitorsSameDisplayFormat",
                    "MouseButtons",
                    "MouseButtonsSwapped",
                    "MousePresent",
                    "MouseWheelPresent",
                    "MouseWheelScrollLines",
                    "NativeMouseWheelSupport",
                    "Network",
                    "PenWindows",
                    "PrimaryMonitorMaximizedWindowSize",
                    "PrimaryMonitorSize",
                    "RightAlignedMenus",
                    "Secure",
                    "ShowSounds",
                    "SmallIconSize",
                    "ToolWindowCaptionButtonSize",
                    "ToolWindowCaptionHeight",
                    "UserDomainName",
                    "UserInteractive",
                    "UserName",
                    "VerticalScrollBarArrowHeight",
                    "VerticalScrollBarThumbHeight",
                    "VerticalScrollBarWidth",
                    "VirtualScreen",
                    "WorkingArea",
               };
          }
     }
     public static string[] Values
     {
          get 
          { 
               return new string[] 
               {
               SystemInformation.ArrangeDirection.ToString(),
               SystemInformation.ArrangeStartingPosition.ToString(),
               SystemInformation.BootMode.ToString(),
               SystemInformation.Border3DSize.ToString(),
               SystemInformation.BorderSize.ToString(),
               SystemInformation.CaptionButtonSize.ToString(),
               SystemInformation.CaptionHeight.ToString(),
               SystemInformation.ComputerName,
               SystemInformation.CursorSize.ToString(),
               SystemInformation.DbcsEnabled.ToString(),
               SystemInformation.DebugOS.ToString(),
               SystemInformation.DoubleClickSize.ToString(),
               SystemInformation.DoubleClickTime.ToString(),
               SystemInformation.DragFullWindows.ToString(),
               SystemInformation.DragSize.ToString(),
               SystemInformation.FixedFrameBorderSize.ToString(),
               SystemInformation.FrameBorderSize.ToString(),
               SystemInformation.HighContrast.ToString(),
               SystemInformation.HorizontalScrollBarArrowWidth.ToString(),
               SystemInformation.HorizontalScrollBarHeight.ToString(),
               SystemInformation.HorizontalScrollBarThumbWidth.ToString(),
               SystemInformation.IconSize.ToString(),
               SystemInformation.IconSpacingSize.ToString(),
               SystemInformation.KanjiWindowHeight.ToString(),
               SystemInformation.MaxWindowTrackSize.ToString(),
               SystemInformation.MenuButtonSize.ToString(),
               SystemInformation.MenuCheckSize.ToString(),
			   SystemInformation.MenuFont.ToString(),
               SystemInformation.MenuHeight.ToString(),
               SystemInformation.MidEastEnabled.ToString(),
               SystemInformation.MinimizedWindowSize.ToString(),
               SystemInformation.MinimizedWindowSpacingSize.ToString(),
               SystemInformation.MinimumWindowSize.ToString(),
               SystemInformation.MinWindowTrackSize.ToString(),
               SystemInformation.MonitorCount.ToString(),
               SystemInformation.MonitorsSameDisplayFormat.ToString(),
               SystemInformation.MouseButtons.ToString(),
               SystemInformation.MouseButtonsSwapped.ToString(),
               SystemInformation.MousePresent.ToString(),
               SystemInformation.MouseWheelPresent.ToString(),
               SystemInformation.MouseWheelScrollLines.ToString(),
               SystemInformation.NativeMouseWheelSupport.ToString(),
               SystemInformation.Network.ToString(),
               SystemInformation.PenWindows.ToString(),
               SystemInformation.PrimaryMonitorMaximizedWindowSize.ToString(),
               SystemInformation.PrimaryMonitorSize.ToString(),
               SystemInformation.RightAlignedMenus.ToString(),
               SystemInformation.Secure.ToString(),
               SystemInformation.ShowSounds.ToString(),
               SystemInformation.SmallIconSize.ToString(),
               SystemInformation.ToolWindowCaptionButtonSize.ToString(),
               SystemInformation.ToolWindowCaptionHeight.ToString(),
               SystemInformation.UserDomainName,
               SystemInformation.UserInteractive.ToString(),
               SystemInformation.UserName,
               SystemInformation.VerticalScrollBarArrowHeight.ToString(),
               SystemInformation.VerticalScrollBarThumbHeight.ToString(),
               SystemInformation.VerticalScrollBarWidth.ToString(),
               SystemInformation.VirtualScreen.ToString(),
               SystemInformation.WorkingArea.ToString(),
               };
          }    
     }
     public static int Count
     {
          get
          {
               return Labels.Length;
          }
     }
     public static float MaxLabelWidth(Graphics grfx, Font font)
     {
          return MaxWidth(Labels, grfx, font);
     }
     public static float MaxValueWidth(Graphics grfx, Font font)
     {
          return MaxWidth(Values, grfx, font);
     }
     static float MaxWidth(string[] astr, Graphics grfx, Font font) 
     {
          float fMax = 0;

          foreach (string str in astr)
               fMax = Math.Max(fMax, grfx.MeasureString(str, font).Width);

          return fMax;
     }
}
