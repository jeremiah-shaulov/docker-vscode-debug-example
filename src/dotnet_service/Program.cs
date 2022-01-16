using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace dotnet_service
{	class Program
	{	static async Task Main(string[] args)
		{	var cancellationSource = new CancellationTokenSource();
			var cancellationToken = cancellationSource.Token;
			Console.CancelKeyPress += (sender, e) =>
			{	cancellationSource.Cancel();
				Console.WriteLine("Service terminated");
			};

			var listener = new TcpListener(new IPEndPoint(IPAddress.Any, 7287));
			listener.Start();
			cancellationToken.Register(listener.Stop);

			Console.WriteLine("Service started on {0}", listener.Server.LocalEndPoint);

			while (!cancellationToken.IsCancellationRequested)
			{	try
				{	var _ = HandleConn(await listener.AcceptTcpClientAsync());
				}
				catch (SocketException) when (cancellationToken.IsCancellationRequested)
				{	break;
				}
				catch (Exception e)
				{	Console.WriteLine("Error handling client: {0}", e.Message);
				}
			}

			async Task HandleConn(TcpClient client)
			{	try
				{	Console.WriteLine("Conn");
					var stream = client.GetStream();
					await stream.WriteAsync(Encoding.ASCII.GetBytes("Hi, i'm dotnet_service\n"));
					var buffer = new byte[1024];
					while (true)
					{	var nRead = await stream.ReadAsync(buffer, 0, buffer.Length);
						if (nRead == 0)
						{	break;
						}
						await stream.WriteAsync(buffer, 0, nRead);
					}
					await stream.WriteAsync(Encoding.ASCII.GetBytes("Bye\n"));
				}
				finally
				{	client.Dispose();
				}
			}
		}
	}
}
