<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class FormMain
    Inherits System.Windows.Forms.Form

    'Das Formular überschreibt den Löschvorgang, um die Komponentenliste zu bereinigen.
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Wird vom Windows Form-Designer benötigt.
    Private components As System.ComponentModel.IContainer

    'Hinweis: Die folgende Prozedur ist für den Windows Form-Designer erforderlich.
    'Das Bearbeiten ist mit dem Windows Form-Designer möglich.  
    'Das Bearbeiten mit dem Code-Editor ist nicht möglich.
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.ButtonScan = New System.Windows.Forms.Button()
        Me.TextBoxFileName = New System.Windows.Forms.TextBox()
        Me.RichTextBoxResult = New System.Windows.Forms.RichTextBox()
        Me.TextBoxDataBase = New System.Windows.Forms.TextBox()
        Me.SuspendLayout()
        '
        'ButtonScan
        '
        Me.ButtonScan.Location = New System.Drawing.Point(513, 12)
        Me.ButtonScan.Name = "ButtonScan"
        Me.ButtonScan.Size = New System.Drawing.Size(76, 46)
        Me.ButtonScan.TabIndex = 0
        Me.ButtonScan.Text = "Scan"
        Me.ButtonScan.UseVisualStyleBackColor = True
        '
        'TextBoxFileName
        '
        Me.TextBoxFileName.Location = New System.Drawing.Point(12, 12)
        Me.TextBoxFileName.Name = "TextBoxFileName"
        Me.TextBoxFileName.Size = New System.Drawing.Size(495, 20)
        Me.TextBoxFileName.TabIndex = 1
        '
        'RichTextBoxResult
        '
        Me.RichTextBoxResult.Location = New System.Drawing.Point(12, 64)
        Me.RichTextBoxResult.Name = "RichTextBoxResult"
        Me.RichTextBoxResult.Size = New System.Drawing.Size(495, 240)
        Me.RichTextBoxResult.TabIndex = 3
        Me.RichTextBoxResult.Text = ""
        '
        'TextBoxDataBase
        '
        Me.TextBoxDataBase.Location = New System.Drawing.Point(12, 38)
        Me.TextBoxDataBase.Name = "TextBoxDataBase"
        Me.TextBoxDataBase.Size = New System.Drawing.Size(495, 20)
        Me.TextBoxDataBase.TabIndex = 4
        '
        'FormMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(601, 318)
        Me.Controls.Add(Me.TextBoxDataBase)
        Me.Controls.Add(Me.RichTextBoxResult)
        Me.Controls.Add(Me.TextBoxFileName)
        Me.Controls.Add(Me.ButtonScan)
        Me.Name = "FormMain"
        Me.Text = "DIE"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents ButtonScan As Button
    Friend WithEvents TextBoxFileName As TextBox
    Friend WithEvents RichTextBoxResult As RichTextBox
    Friend WithEvents TextBoxDataBase As TextBox
End Class
