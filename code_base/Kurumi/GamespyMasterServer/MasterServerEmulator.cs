using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace GamespyMasterServerTCP {
    class MasterServerEmulator {

        private static Socket _gs2002;
        private static Socket _gs2004;
        private const int gs2002Port = 28900;
        private const int gs2004Port = 28910;
        private static byte[] payloadGS2002Bytes;
        private static byte[] payloadGS2004Bytes;

        static async Task Main(string[] args) {

            _gs2002 = CreateSocket(gs2002Port);
            _gs2004 = CreateSocket(gs2004Port);

            Console.WriteLine($"Gamespy keowu v1.2.0\nServer is listening on ports {gs2002Port} and {gs2004Port}");

            await Task.WhenAll(
                Task.Run(async () => await HandleClients(_gs2002, HandleGS2002Clients)),
                Task.Run(async () => await HandleClients(_gs2004, HandleGS2004Clients))
            );

        }

        static async Task HandleClients(Socket socket, Func<Socket, Task> handler) {

            while (true) {

                try {

                    var clientSocket = await socket.AcceptAsync();
                    Task.Run(() => handler(clientSocket));

                }
                catch (Exception ex) {

                    Console.WriteLine($"Error: {ex.Message}");

                }
            }
        }

        static Socket CreateSocket(int port) {

            var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp) {

                SendTimeout = 1000,
                ReceiveTimeout = 1000,
                SendBufferSize = 65535,
                ReceiveBufferSize = 65535,
                ExclusiveAddressUse = true,
                LingerState = new LingerOption(false, 0)

            };

            var endPoint = new IPEndPoint(IPAddress.Any, port);
            socket.Bind(endPoint);
            socket.Listen(10);

            return socket;
        }

        static async Task HandleGS2002Clients(Socket clientSocket) {

            try {

                payloadGS2002Bytes = File.ReadAllBytes(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "payload.bin"));
                Console.WriteLine($"New Client {clientSocket.RemoteEndPoint} connected to Gamespy2002.");

                byte[] helloMessage = Encoding.UTF8.GetBytes("\\basic\\secure\\MASTER");
                await clientSocket.SendAsync(new ArraySegment<byte>(helloMessage), SocketFlags.None);

                var buffer = new byte[65535];
                while (true) {

                    var receivedBytes = await clientSocket.ReceiveAsync(new ArraySegment<byte>(buffer), SocketFlags.None);

                    if (receivedBytes == 0) {

                        Console.WriteLine($"Client {clientSocket.RemoteEndPoint} disconnected from Gamespy2002.");
                        
                        break;
                    }

                    var receivedMessage = Encoding.UTF8.GetString(buffer, 0, receivedBytes);

                    if (receivedMessage.Contains("bfield1942\\final")) {

                        await clientSocket.SendAsync(new ArraySegment<byte>(payloadGS2002Bytes), SocketFlags.None);
                        
                        break;
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

        static async Task HandleGS2004Clients(Socket clientSocket) {

            var buffer = new byte[65535];
            var receivedBytes = clientSocket.Receive(new ArraySegment<byte>(buffer), SocketFlags.None);
            var receivedMessage = Encoding.UTF8.GetString(buffer, 0, receivedBytes);
            if (receivedMessage.Contains("bfvietnam")) {

                //PAYLOAD 3 DO NOT HAVE A PARSER YET, TODO: CREATE A PARSE TO INSERT FRAMES BASED ON 0X15 FLAG ENDING WITH 0X00 0XFFFFFFF MAGIC
                payloadGS2004Bytes = File.ReadAllBytes(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "payload3.bin"));
                await clientSocket.SendAsync(new ArraySegment<byte>(payloadGS2004Bytes), SocketFlags.None);

            }else {

                payloadGS2004Bytes = File.ReadAllBytes(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "payload2.bin"));
                await clientSocket.SendAsync(new ArraySegment<byte>(payloadGS2004Bytes), SocketFlags.None);

            }

            Console.WriteLine($"New Client {clientSocket.RemoteEndPoint} connected to Gamespy2004.");
        }
    }
}
