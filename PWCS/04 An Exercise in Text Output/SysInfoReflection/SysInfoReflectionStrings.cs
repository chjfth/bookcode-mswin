//-------------------------------------------------------
// SysInfoReflectionStrings.cs © 2001 by Charles Petzold
//-------------------------------------------------------
using Microsoft.Win32;
using System;
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;

class SysInfoReflectionStrings
{
                                                            // Fields
     static bool     bValidInfo = false;
     static int      iCount;
     static string[] astrLabels;
     static string[] astrValues;
                                                            // Constructor
     static SysInfoReflectionStrings()
     {
          SystemEvents.UserPreferenceChanged += 
               new UserPreferenceChangedEventHandler(UserPreferenceChanged);

          SystemEvents.DisplaySettingsChanged +=
               new EventHandler(DisplaySettingsChanged);
     }
                                                            // Properties
     public static string[] Labels
     {
          get
          {
               GetSysInfo();
               return astrLabels;
          }
     }
     public static string[] Values
     {
          get
          {
               GetSysInfo();
               return astrValues;
          }
     }
     public static int Count
     {
          get 
          { 
               GetSysInfo();
               return iCount;
          }
     }
                                                            // Event handlers
     static void UserPreferenceChanged(object obj, 
                                       UserPreferenceChangedEventArgs ea)
     {
          bValidInfo = false;
     }
     static void DisplaySettingsChanged(object obj, EventArgs ea)
     {
          bValidInfo = false;
     }
                                                            // Methods
     static void GetSysInfo()
     {
          if(bValidInfo)
               return;

               // Get property information for SystemInformation class.

          Type type = typeof(SystemInformation);
          PropertyInfo[] apropinfo = type.GetProperties();

               // Count the number of static readable properties.

          iCount = 0;
          foreach (PropertyInfo pi in apropinfo)
          {
               if(pi.CanRead && pi.GetGetMethod().IsStatic)
                    iCount++;
          }
               // Allocate string arrays.

          astrLabels = new string[iCount];
          astrValues = new string[iCount];

               // Loop through the property information classes again.

          iCount = 0;
          foreach (PropertyInfo pi in apropinfo)
          {
               if(pi.CanRead && pi.GetGetMethod().IsStatic)
               {
                         // Get the property names and values.

                    astrLabels[iCount] = pi.Name;
                    astrValues[iCount] = pi.GetValue(type, null).ToString();
                    iCount++;
               }
          }
		  Array.Sort(astrLabels, astrValues);
          bValidInfo = true;
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

          GetSysInfo();

          foreach (string str in astr)
               fMax = Math.Max(fMax, grfx.MeasureString(str, font).Width);

          return fMax;
     }
}
