<?php

if (isset($_GET['info']))
{	phpinfo();
}
else
{	echo "<div>Hi, i'm php_fpm_service</div>";

	ask_service('deno_service', 64840);
	ask_service('node_service', 12982);
	ask_service('rust_service', 54329);
	ask_service('swift_service', 15880);
	ask_service('dotnet_service', 7287);
	ask_service('python_service', 8497);
	ask_service('c_service', 8543);
	ask_service('java_service', 27712);
}

function ask_service($host, $port)
{	echo "<div>Here's what $host says:</div>";
	$fh = stream_socket_client("tcp://$host:$port", $errno, $errstr, 2);
	if (!$fh)
	{	echo "<div style='color:red'>$errstr</div>";
	}
	else
	{	stream_set_blocking($fh, false);
		fwrite($fh, "Blah ");
		stream_socket_shutdown($fh, STREAM_SHUT_WR);
		stream_set_blocking($fh, true);
		echo "<div style='color:blue'>", stream_get_contents($fh), "</div>";
	}
}
