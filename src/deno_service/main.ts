import {writeAll} from './private/deps.ts';

const encoder = new TextEncoder;

// 1. Start server
const server = Deno.listen({transport: 'tcp', hostname: 'deno_service', port: 64840});
console.log('Service started on', server.addr);

// 2. Wait for termination request
Promise.race
(	[	Deno.signal('SIGINT'),
		Deno.signal('SIGTERM'),
	]
).then(() => server.close());

// 3. Serve incoming connections
for await (const conn of server)
{	queueMicrotask(() => handleConn(conn));
}

// 4. This is how to serve each incoming connection
async function handleConn(conn: Deno.Conn)
{	try
	{	console.log('Conn');
		await writeAll(conn, encoder.encode(`Hi, i'm deno_service\n`));
		const buf = new Uint8Array(1024);
		while (true)
		{	const n = await conn.read(buf);
			if (n == null)
			{	break;
			}
			await writeAll(conn, buf.subarray(0, n));
		}
		await writeAll(conn, encoder.encode(`Bye\n`));
	}
	catch (e)
	{	console.error(e);
	}
	finally
	{	conn.close();
	}
}

// 5. Done
console.log('Service terminated');
