package com.none.java_service;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousServerSocketChannel;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;

class Conf
{	static final int PORT = 27712;
	static final int BUFFER_SIZE = 1024;
}

public class App
{	public static void main(String[] args)
	{	try
		(	final AsynchronousServerSocketChannel listener = AsynchronousServerSocketChannel.open();
		)
		{	// Listen
			listener.bind(new InetSocketAddress("java_service", Conf.PORT));
			System.out.format("Service started on %d\n", Conf.PORT);

			// Accept connections and handle them asynchronously
			while (true)
			{	Client.handleConn(listener.accept().get());
			}
		}
		catch (Throwable e)
		{	e.printStackTrace();
		}
		// Done
		System.out.println("Service terminated");
	}
}

/**	Class that handles incomming connection.
 **/
class Client implements CompletionHandler<Integer, Void>
{	private enum State
	{	INITIAL,
		AFTER_SEND,
		AFTER_RECV,
		AFTER_SEND_LAST,
	}

	private AsynchronousSocketChannel conn;
	private State state = State.INITIAL;
	private ByteBuffer buffer = ByteBuffer.allocateDirect(Conf.BUFFER_SIZE);

	private Client(AsynchronousSocketChannel conn)
	{	this.conn = conn;
	}

	public static void handleConn(AsynchronousSocketChannel conn)
	{	Client client = new Client(conn);
		client.next(null);
	}

	public void next(Throwable exception)
	{	if (exception != null)
		{	try
			{	conn.close();
			}
			catch (IOException e)
			{	e.printStackTrace();
			}
			exception.printStackTrace();
			return;
		}

		switch (state)
		{	case INITIAL:
				System.out.println("Conn");
				buffer.put("Hi, i'm java_service\n".getBytes());
				state = State.AFTER_SEND;
				conn.write(buffer.flip(), null, this);
				break;
			case AFTER_SEND:
				buffer.clear();
				state = State.AFTER_RECV;
				conn.read(buffer, null, this);
				break;
			case AFTER_RECV:
				buffer.flip();
				if (buffer.limit() == 0)
				{	// EOF reached
					buffer.clear();
					buffer.put("Bye\n".getBytes());
					state = State.AFTER_SEND_LAST;
					conn.write(buffer.flip(), null, this);
				}
				else
				{	state = State.AFTER_SEND;
					conn.write(buffer, null, this);
				}
				break;
			case AFTER_SEND_LAST:
				try
				{	conn.close();
				}
				catch (IOException e)
				{	e.printStackTrace();
				}
		}
	}

	@Override
	public void completed(Integer arg0, Void arg1)
	{	next(null);
	}

	@Override
	public void failed(Throwable arg0, Void arg1)
	{	next(arg0);
	}
}
