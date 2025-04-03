Attribute VB_Name = "Module1"
Option Explicit

Global abort As Boolean

Private Declare Function lstrlenW Lib "kernel32" (ByVal lpString As Long) As Long
Private Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (pDest As Any, pSrc As Any, ByVal ByteLen As Long)

'Return 0 to Abort, 1 to continue
Function DIE_VB_CALLBACK(ByVal curSigName As Long, ByVal curSigindex As Long, ByVal maxSigs As Long) As Long
    
    On Error Resume Next
    Dim sz As Long, curSig As String
    
    Form1.pb.Max = maxSigs
    Form1.pb.Value = curSigindex
    
    If curSigName <> 0 Then
        sz = lstrlenW(curSigName)
        If sz > 0 And sz < 2000 Then
            curSig = String(sz, 0)
            CopyMemory ByVal StrPtr(curSig), ByVal curSigName, sz
            Form1.lblSig = Replace(curSig, Chr(0), Empty)
        End If
    End If
    
    DoEvents
    DIE_VB_CALLBACK = IIf(abort, 0, 1)
    
End Function


Function GetParentFolder(path) As String
    Dim tmp() As String, ub  As String
    tmp = Split(path, "\")
    ub = tmp(UBound(tmp))
    GetParentFolder = Replace(Join(tmp, "\"), "\" & ub, "")
End Function

Function FileExists(path As String) As Boolean
  On Error GoTo hell
    
  If Len(path) = 0 Then Exit Function
  If Right(path, 1) = "\" Then Exit Function
  If Dir(path, vbHidden Or vbNormal Or vbReadOnly Or vbSystem) <> "" Then FileExists = True
  
  Exit Function
hell: FileExists = False
End Function

Function FolderExists(path As String) As Boolean
  On Error GoTo hell
  Dim tmp As String
  tmp = path & "\"
  If Len(tmp) = 1 Then Exit Function
  If Dir(tmp, vbDirectory) <> "" Then FolderExists = True
  Exit Function
hell:
End Function


