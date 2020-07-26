<?php 

`stty -F /dev/ttyUSB0 115200`;
$fp = fopen('/dev/ttyUSB0','r+');
stream_set_timeout($fp, 0, 1000);
stream_set_blocking($fp,0);

$data = file_get_contents("main.hex");
foreach (explode("\n", $data) as $line)
{
	if (trim($line) == "")
		continue; 

	$line = strtoupper($line);

	fputs($fp,$line);

	$read = ""; 

	if (strpos($line, ":00000001") === false)
	{
		$start = microtime(true);
		while (strpos($read, "#") === false)
		{
			$read .= fread($fp,128);

			if (microtime(true) - $start > 1.0)
			{
				echo "Timeout waiting for " . $line . "\n";
				continue;
			}
		}
		echo $read;
	}
	else
	{
		echo "done \o/";
		sleep(1);
		$read .= fread($fp,4096);
		echo $read;
	}
	
}


fclose($fp);
