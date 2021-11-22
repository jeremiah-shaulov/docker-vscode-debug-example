use tokio::net::{TcpListener, TcpStream};
use tokio::io::{AsyncReadExt, AsyncWriteExt};

const LISTEN: &str = "rust_service:9023";
const BUFFER_SIZE: usize = 1024;

#[tokio::main]
async fn main() -> std::io::Result<()>
{	let listener = TcpListener::bind(LISTEN).await?;
	println!("Service started on {}", LISTEN);
	while let Ok((stream, _socket_addr)) = listener.accept().await
	{	tokio::spawn
		(	async move
			{	handle_conn(stream).await;
			}
		);
	}
	println!("Service terminated");
	Ok(())
}

async fn handle_conn(mut stream: TcpStream)
{	println!("Conn");
	if let Err(err) = stream.write_all(b"Hi, i'm rust_service\n").await
	{	eprintln!("Failed to write to socket: {:?}", err);
		return;
	}
	let mut buf = [0u8; BUFFER_SIZE];
	loop
	{	// Read
		let n = match stream.read(&mut buf).await
		{	Ok(n) if n == 0 => break, // Socket closed
			Ok(n) => n,
			Err(err) =>
			{	eprintln!("Failed to read from socket: {:?}", err);
				return;
			}
		};
		// Write
		if let Err(err) = stream.write_all(&buf[0 .. n]).await
		{	eprintln!("Failed to write to socket: {:?}", err);
			return;
		}
	}
	// Socket closed
	if let Err(err) = stream.write_all(b"Bye\n").await
	{	eprintln!("Failed to write to socket: {:?}", err);
		return;
	}
}
