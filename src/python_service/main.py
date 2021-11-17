#!/usr/bin/env python

import asyncio

PORT = 8497
BUFFER_SIZE = 1024

async def handle_echo(reader, writer):
	print('Conn')
	writer.write(bytes("Hi, i'm python_service\n", 'utf-8'))
	while True:
		data = await reader.read(BUFFER_SIZE)
		if len(data) == 0:
			break
		writer.write(data)
		await writer.drain()

	writer.write(bytes('Bye\n', 'utf-8'))
	await writer.drain()
	writer.close()

loop = asyncio.new_event_loop()
asyncio.set_event_loop(loop)
coro = asyncio.start_server(handle_echo, 'python_service', PORT, limit=BUFFER_SIZE)
server = loop.run_until_complete(coro)

# Serve requests until Ctrl+C is pressed
print(f'Service started on {server.sockets[0].getsockname()}')
try:
	loop.run_forever()
except KeyboardInterrupt:
	pass

# Done
server.close()
loop.run_until_complete(server.wait_closed())
loop.close()
print('Service terminated')
