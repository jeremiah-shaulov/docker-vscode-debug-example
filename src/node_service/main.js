import {createServer} from 'net';

createServer({allowHalfOpen: true}).on('connection', handleConn).listen
(	12982,
	function()
	{	console.log('Service started on', this.address());
	}
);

function handleConn(conn)
{	console.log('Conn');
	conn.on
	(	'data',
		data =>
		{	conn.write(data);
		}
	).once
	(	'end',
		() =>
		{	conn.end(`Bye\n`);
		}
	).on
	(	'error',
		error =>
		{	console.error(error);
		}
	).write(`Hi, i'm node_service\n`);
}
