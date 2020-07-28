<?php 

if ($argc != 3)
	die("usage: ./send.php /dev/ttyUSB0 main.bin\n");

$serial = $argv[1];
$inputFile = $argv[2];

if (!file_exists($serial))
	die("serial device doesn't exist!\n");

if (!file_exists($inputFile))
	die("input file doesn't exist\n");

$cmd = 'objcopy --input-target=binary --output-target=ihex '. escapeshellarg($inputFile) .' /dev/stdout';

$data = `$cmd`;

$serialEscaped = escapeshellarg($serial);
`stty -F $serialEscaped 115200`;

$fp = fopen($serial,'r+');
stream_set_timeout($fp, 0, 1000);
stream_set_blocking($fp,0);

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
