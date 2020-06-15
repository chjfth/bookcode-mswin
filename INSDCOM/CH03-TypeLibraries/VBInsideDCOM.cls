VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "VBInsideDCOM"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Implements InsideDCOM

Private Function InsideDCOM_Sum(ByVal x As Long, ByVal y As Long) As Long
InsideDCOM_Sum = x + y
End Function
