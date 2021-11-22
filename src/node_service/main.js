import {createServer} from 'net';

createServer({allowHalfOpen: true}).on('connection', handleConn).listen
(	12982,
	'node_service',
	function()
	{	console.log('Service started on', this.address());
	}
);

function handleConn(conn)
{	console.log('Conn');
	conn.once
	(	'end',
		() =>
		{	conn.end(`Bye\n`);
		}
	).on
	(	'error',
		error =>
		{	console.error(error);
		}
	);
	conn.write(`Hi, i'm node_service\n`);
	conn.pipe(conn);
}
