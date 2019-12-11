Imports System.Threading
Public Class Form1
    Delegate Sub ListBoxDelegate(ByVal command As Integer, ByVal myStr As String)
    Dim ListBoxDel As New ListBoxDelegate(AddressOf ListBoxDelMethod)
    Dim ListBoxSelect As New ListBoxDelegate(AddressOf ListBoxSelectMethod)
    Dim ListBoxClear As New ListBoxDelegate(AddressOf ListBoxClearMethod)



    Dim PC_Comm As New Thread(AddressOf PC_Comm_method)
    Dim ShowFiles As Integer = 1
    Dim PlaySong As Integer = 2
    Dim PauseSong As Integer = 3
    Dim StopSong As Integer = 4
    Dim StartFileList As Integer = 5
    Dim EndFileList As Integer = 6

    Dim ShowFilesStr As String = "1"
    Dim PlaySongStr As String = "2"
    Dim PauseSongStr As String = "3"
    Dim StopSongStr As String = "4"
    Dim StartFileListStr As String = "5"
    Dim EndFileListStr As String = "6"
    Dim StopStr As String = "7"

    Dim Files As Integer = 0

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        'Try to open the seial port 
        Try
            SerialPort1.Open()
        Catch
            Debug.WriteLine("Failed to open serial port")
        End Try

        ' Make this a background thread so it automatically
        ' aborts when the main program stops.
        PC_Comm.IsBackground = True
        ' Set the thread priority to lowest
        PC_Comm.Priority = ThreadPriority.Lowest
        ' Start the thread
        PC_Comm.Start()

    End Sub
    ' Delegate function that accesses the ListBox object
    ' command = 2 - clear the contents of the ListBox
    ' command = 3 - add the string to the ListBox
    Public Sub ListBoxDelMethod(ByVal command As Integer, ByVal myStr As String)

        If command = StartFileList Then
            File_List.Items.Clear()
        ElseIf command = EndFileList Then
            File_List.Items.Add(myStr)
        End If
    End Sub

    Public Sub ListBoxSelectMethod(ByVal command As Integer, ByVal myStr As String)
        Randomize()
        Dim value As Integer = CInt(Int((Files * Rnd()) + 1))
        File_List.SetSelected(value, True)
    End Sub

    Public Sub ListBoxClearMethod(ByVal command As Integer, ByVal myStr As String)
        File_List.ClearSelected()
    End Sub

    ' Thread that monitors the receive items on the serial port
    Private Sub PC_Comm_method()
        Dim str As String
        While 1
            If SerialPort1.IsOpen Then
                Try
                    str = SerialPort1.ReadLine() ' Wait for start string
                Catch ex As Exception
                End Try

                'String.Compare return values:
                ' Less than zero: strA precedes strB in the sort order.
                'Zero" strA occurs in the same position as strB in the sort order.
                'Greater than zero: strA follows strB in the sort order.
                If Not String.Compare(str, StartFileListStr) Then
                    Files = 0
                    ' Received a StartFileList response
                    ' clear the list
                    ' Use the delegate to access the ListBox
                    File_List.Invoke(ListBoxDel, StartFileList, "")
                    ' get next string
                    Try
                        str = SerialPort1.ReadLine() ' read file name
                    Catch ex As Exception
                    End Try
                    While String.Compare(str, EndFileListStr)
                        ' The read string is a file name and not the EndFileList
                        File_List.Invoke(ListBoxDel, EndFileList, str)
                        Try
                            str = SerialPort1.ReadLine() ' read file name
                        Catch ex As Exception
                        End Try
                        Files = Files + 1
                    End While

                    File_List.Invoke(ListBoxSelect, EndFileList, str)
                End If
                ' While loop ends when a 3 is received

            Else
                Threading.Thread.Sleep(500)
            End If
        End While
    End Sub

    Private Sub PlayButton_Click(sender As Object, e As EventArgs) Handles PlayButton.Click
        Dim b(1) As Byte
        b(0) = 0
        Dim file As String
        Dim i As Integer = 0
        ' A value of negative one (-1) is returned if no item is selected
        If Not (File_List.SelectedIndex = -1) Then
            SerialPort1.Write(PlaySongStr, 0, 1)
            file = File_List.SelectedItem

            While (i < file.Length)
                SerialPort1.Write(file(i))
                i = i + 1
            End While

            SerialPort1.Write(b, 0, 1) ' New Line character at the end of the string

            SongPlaying.Text = file
        End If
    End Sub

    Private Sub PauseButton_Click(sender As Object, e As EventArgs) Handles PauseButton.Click
        If SerialPort1.IsOpen Then
            ' Send Show_Files command
            SerialPort1.Write(PauseSongStr, 0, 1)
        End If
    End Sub

    Private Sub StopButton_Click(sender As Object, e As EventArgs) Handles StopButton.Click
        If SerialPort1.IsOpen Then
            ' Send Show_Files command
            SerialPort1.Write(StopSongStr, 0, 1)
        End If

        File_List.Invoke(ListBoxClear, EndFileList, "")

        File_List.Invoke(ListBoxSelect, EndFileList, "")
    End Sub

    Private Sub RefreshButton_Click(sender As Object, e As EventArgs) Handles RefreshButton.Click
        If SerialPort1.IsOpen Then
            ' Send Show_Files command
            SerialPort1.Write(ShowFilesStr, 0, 1)
        End If
    End Sub

End Class
