VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   2508
   ClientLeft      =   48
   ClientTop       =   324
   ClientWidth     =   3744
   LinkTopic       =   "Form1"
   ScaleHeight     =   2508
   ScaleWidth      =   3744
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "Command1"
      Height          =   492
      Left            =   1800
      TabIndex        =   0
      Top             =   840
      Width           =   1332
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
Dim myRef As New Component.InsideDCOM
MsgBox "myRef.Sum(5, 6) returns " & myRef.Sum(5, 6)
End Sub
