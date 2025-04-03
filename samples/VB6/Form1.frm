VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "DieLib vb6 Sample"
   ClientHeight    =   4680
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   9330
   LinkTopic       =   "Form1"
   ScaleHeight     =   4680
   ScaleWidth      =   9330
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton cmdScan 
      Caption         =   "Scan"
      Height          =   375
      Left            =   6960
      TabIndex        =   5
      Top             =   60
      Width           =   855
   End
   Begin VB.TextBox txtFile 
      Height          =   315
      Left            =   660
      OLEDropMode     =   1  'Manual
      TabIndex        =   4
      Top             =   60
      Width           =   6075
   End
   Begin VB.TextBox Text1 
      Height          =   3435
      Left            =   180
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   2
      Top             =   1080
      Width           =   8895
   End
   Begin MSComctlLib.ProgressBar pb 
      Height          =   255
      Left            =   180
      TabIndex        =   1
      Top             =   480
      Width           =   9015
      _ExtentX        =   15901
      _ExtentY        =   450
      _Version        =   393216
      Appearance      =   1
   End
   Begin VB.CommandButton cmdAbort 
      Caption         =   "Abort"
      Height          =   375
      Left            =   8040
      TabIndex        =   0
      Top             =   60
      Width           =   975
   End
   Begin VB.Label lblSig 
      Height          =   255
      Left            =   240
      TabIndex        =   6
      Top             =   780
      Width           =   8895
   End
   Begin VB.Label Label1 
      Caption         =   "Scan"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   495
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Const DIE_DEEPSCAN = &H1
Const DIE_HEURISTICSCAN = &H2
Const DIE_ALLTYPESSCAN = &H4
Const DIE_RECURSIVESCAN = &H8
Const DIE_VERBOSE = &H10
Const DIE_AGGRESSIVESCAN = &H20
Const DIE_RESULTASXML = &H10000
Const DIE_RESULTASJSON = &H20000
Const DIE_RESULTASTSV = &H40000
Const DIE_RESULTASCSV = &H80000

Private hDieDll As Long
Private dieDB As String

'http://ntinfo.biz/
'https://github.com/horsicq/die_library
Private Declare Function DIE_VB_ScanFile Lib "die.dll" Alias "_DIE_VB_ScanFile@20" ( _
    ByVal fileName As Long, ByVal flags As Long, ByVal db As Long, _
    ByVal buf As Long, ByVal bufSz As Long) As Long

Private Declare Function DIE_VB_ScanFileCallback Lib "die.dll" Alias "_DIE_VB_ScanFileCallback@24" ( _
    ByVal fileName As Long, ByVal flags As Long, ByVal db As Long, _
    ByVal buf As Long, ByVal bufSz As Long, ByVal pfnCallback As Long) As Long
    
Private Declare Function LoadLibrary Lib "kernel32" Alias "LoadLibraryA" (ByVal lpLibFileName As String) As Long
Private Declare Function SetDllDirectoryA Lib "kernel32" (ByVal lpPathName As String) As Long

Private Sub cmdAbort_Click()
    abort = True
End Sub

Private Sub Form_Load()
    txtFile = "C:\Windows\explorer.exe"
End Sub

Function initDie() As Boolean
    
    Dim dll As String

    If hDieDll <> 0 Then
        initDie = True
        Exit Function
    End If
    
    dieDB = App.path & "\db"
    dll = App.path & "\die.dll"
    
    If Not FileExists(dll) Then
        dll = App.path & "\..\die.dll"
        If Not FileExists(dll) Then
            dll = App.path & "\..\..\die.dll"
            If Not FileExists(dll) Then
                Text1 = "die.dll not found: " & dll
                Exit Function
            End If
        End If
    End If
    
    If Not FolderExists(dieDB) Then
        dieDB = App.path & "\..\db"
        If Not FolderExists(dieDB) Then
            dieDB = App.path & "\..\..\db"
            If Not FolderExists(dieDB) Then
                Text1 = "db not found: " & dieDB
                Exit Function
            End If
        End If
    End If
    
    SetDllDirectoryA GetParentFolder(dll)
    hDieDll = LoadLibrary(dll)
    
    If hDieDll <> 0 Then initDie = True
    
End Function

Private Sub cmdScan_Click()

    Dim exe As String
     
    abort = False
    
    If Not initDie() Then
        Text1 = "Could not initDie()"
        Exit Sub
    End If
   
    exe = txtFile
    If Not FileExists(exe) Then
        Text1 = "File not found: " & exe
        Exit Sub
    End If
    
    flags = DIE_ALLTYPESSCAN Or DIE_VERBOSE
    buf = String(2000, Chr(0))
    
    Text1 = "Starting die scan"
    v = DIE_VB_ScanFileCallback(StrPtr(exe), flags, StrPtr(dieDB), StrPtr(buf), Len(buf), AddressOf DIE_VB_CALLBACK)
    
    a = InStr(buf, Chr(0))
    If a > 0 Then buf = Left(buf, a - 1)
    buf = Replace(buf, vbLf, vbCrLf)
    Text1 = buf
    
    pb.Value = 0
    lblSig.Caption = Empty
    
End Sub

Private Sub txtFile_OLEDragDrop(Data As DataObject, Effect As Long, Button As Integer, Shift As Integer, X As Single, Y As Single)
    On Error Resume Next
    Dim f As String
    f = Data.Files(1)
    If FileExists(f) Then txtFile = f
End Sub
