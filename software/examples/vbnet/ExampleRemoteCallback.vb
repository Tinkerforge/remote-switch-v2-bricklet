Imports System
Imports Tinkerforge

Module ExampleRemoteCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your Remote Switch Bricklet 2.0

    ' Callback subroutine for remote status a callback
    Sub RemoteStatusACB(ByVal sender As BrickletRemoteSwitchV2, ByVal houseCode As Byte, _
                        ByVal receiverCode As Byte, ByVal switchTo As Byte, _
                        ByVal repeats As Integer)
        Console.WriteLine("House Code: " + houseCode.ToString())
        Console.WriteLine("Receiver Code: " + receiverCode.ToString())

        If switchTo = BrickletRemoteSwitchV2.SWITCH_TO_OFF Then
            Console.WriteLine("Switch To: Off")
        Else If switchTo = BrickletRemoteSwitchV2.SWITCH_TO_ON Then
            Console.WriteLine("Switch To: On")
        End If

        Console.WriteLine("Repeats: " + repeats.ToString())
        Console.WriteLine("")
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim rs As New BrickletRemoteSwitchV2(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Configure to receive from remote type A with minimum repeats set to 1 and enable callback
        rs.SetRemoteConfiguration(BrickletRemoteSwitchV2.REMOTE_TYPE_A, 1, True)

        ' Register remote status a callback to subroutine RemoteStatusACB
        AddHandler rs.RemoteStatusACallback, AddressOf RemoteStatusACB

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
