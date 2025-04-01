Imports System.Runtime.InteropServices 'for DllImport()
Imports System.Text

Public Class FormMain
    <DllImport("die.dll", CharSet:=CharSet.Unicode,
               CallingConvention:=CallingConvention.stdcall, EntryPoint:="DIE_VB_ScanFile")>
    Shared Function DIE_VB_ScanFile(lpFileName As String, nFlags As Integer, lpDataBase As String, lpBuffer As String, nBufferSize As Integer) As Integer
    End Function

    Private Sub FormMain_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        TextBoxFileName.Text = "C:\Windows\notepad.exe"
        TextBoxDataBase.Text = "$app/db"
    End Sub
    Private Sub ButtonScan_Click(sender As Object, e As EventArgs) Handles ButtonScan.Click
        Dim sFileName As String = TextBoxFileName.Text
        Dim sDataBase As String = TextBoxDataBase.Text
        Dim nBufferSize = 10000
        Dim sBuffer As String
        sBuffer = Space$(nBufferSize)
        Dim nResult As Integer = DIE_VB_ScanFile(sFileName, 0, sDataBase, sBuffer, nBufferSize - 1)
        sBuffer = sBuffer.Substring(0, nResult)
        RichTextBoxResult.Text = sBuffer

    End Sub
End Class
