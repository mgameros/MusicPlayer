<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.PlayButton = New System.Windows.Forms.Button()
        Me.PauseButton = New System.Windows.Forms.Button()
        Me.StopButton = New System.Windows.Forms.Button()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.SongPlaying = New System.Windows.Forms.Label()
        Me.File_List = New System.Windows.Forms.ListBox()
        Me.RefreshButton = New System.Windows.Forms.Button()
        Me.SerialPort1 = New System.IO.Ports.SerialPort(Me.components)
        Me.SuspendLayout()
        '
        'PlayButton
        '
        Me.PlayButton.Location = New System.Drawing.Point(278, 12)
        Me.PlayButton.Name = "PlayButton"
        Me.PlayButton.Size = New System.Drawing.Size(49, 27)
        Me.PlayButton.TabIndex = 0
        Me.PlayButton.Text = "Play"
        Me.PlayButton.UseVisualStyleBackColor = True
        '
        'PauseButton
        '
        Me.PauseButton.Location = New System.Drawing.Point(333, 12)
        Me.PauseButton.Name = "PauseButton"
        Me.PauseButton.Size = New System.Drawing.Size(63, 27)
        Me.PauseButton.TabIndex = 1
        Me.PauseButton.Text = "Pause"
        Me.PauseButton.UseVisualStyleBackColor = True
        '
        'StopButton
        '
        Me.StopButton.Location = New System.Drawing.Point(402, 12)
        Me.StopButton.Name = "StopButton"
        Me.StopButton.Size = New System.Drawing.Size(49, 27)
        Me.StopButton.TabIndex = 2
        Me.StopButton.Text = "Stop"
        Me.StopButton.UseVisualStyleBackColor = True
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(9, 21)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(119, 17)
        Me.Label1.TabIndex = 3
        Me.Label1.Text = "Currently Playing:"
        '
        'SongPlaying
        '
        Me.SongPlaying.AutoSize = True
        Me.SongPlaying.Location = New System.Drawing.Point(134, 21)
        Me.SongPlaying.Name = "SongPlaying"
        Me.SongPlaying.Size = New System.Drawing.Size(51, 17)
        Me.SongPlaying.TabIndex = 4
        Me.SongPlaying.Text = "Label2"
        '
        'File_List
        '
        Me.File_List.FormattingEnabled = True
        Me.File_List.ItemHeight = 16
        Me.File_List.Location = New System.Drawing.Point(12, 45)
        Me.File_List.Name = "File_List"
        Me.File_List.Size = New System.Drawing.Size(438, 276)
        Me.File_List.TabIndex = 5
        '
        'RefreshButton
        '
        Me.RefreshButton.Location = New System.Drawing.Point(196, 327)
        Me.RefreshButton.Name = "RefreshButton"
        Me.RefreshButton.Size = New System.Drawing.Size(69, 27)
        Me.RefreshButton.TabIndex = 6
        Me.RefreshButton.Text = "Refresh"
        Me.RefreshButton.UseVisualStyleBackColor = True
        '
        'SerialPort1
        '
        Me.SerialPort1.PortName = "COM5"
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(8.0!, 16.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(462, 363)
        Me.Controls.Add(Me.RefreshButton)
        Me.Controls.Add(Me.File_List)
        Me.Controls.Add(Me.SongPlaying)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.StopButton)
        Me.Controls.Add(Me.PauseButton)
        Me.Controls.Add(Me.PlayButton)
        Me.Name = "Form1"
        Me.Text = "Music Player"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents PlayButton As Button
    Friend WithEvents PauseButton As Button
    Friend WithEvents StopButton As Button
    Friend WithEvents Label1 As Label
    Friend WithEvents SongPlaying As Label
    Friend WithEvents File_List As ListBox
    Friend WithEvents RefreshButton As Button
    Friend WithEvents SerialPort1 As IO.Ports.SerialPort
End Class
