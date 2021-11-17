use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::thread;

fn main() -> std::io::Result<()>
{	let listener = TcpListener::bind("rust_service:54329")?;
	println!("Service started on {:?}", listener.local_addr());

	for stream in listener.incoming()
	{	let stream = stream?;
		thread::spawn(move || handle_conn(stream));
	}

	println!("Service terminated");
	Ok(())
}

fn handle_conn(mut stream: TcpStream)
{	println!("Conn");
	writeln!(stream, "Hi, i'm rust_service").unwrap();
	let mut buf = [0u8; 1024];
	loop
	{	let len = stream.read(&mut buf).unwrap();
		if len == 0
		{	break;
		}
		stream.write_all(&mut buf[.. len]).unwrap();
	}
	writeln!(stream, "Bye").unwrap();
}
