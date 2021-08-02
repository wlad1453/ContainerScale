<!DOCTYPE html>
<html>
<head>
<style>
body { background-color: #c4e3ed;
	font-family: arial;
	
 	}
.tempData {
	table-layout: fixed;
	width: 840px;
	border-collapse: collapse;  
	white-space: nowrap;
	overflow: hidden;
}
.row-ID {width: 10%;}
.row-DateTime {width: 15%;}
.row-Data {width: 12%;}

table, td, th {
  border: 2px solid black;
  padding: 5px;   
  align: center;
  text-align: center;
}
tr:nth-child(even){background-color: #e4f3fe}

</style>
</head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<body>
<div align="center"><h3>Container scale values</h3></div>

<div align="center" height="800" width="940" style="overflow: hidden; overflow-y:auto;">
<?php

	require 'scale_db/DB_functions.php';

	read_db($conn);

/*
	function db_connect(&$cred) { //Passing arguments by reference
		// Create connection
		$conn = new mysqli($cred['server'], $cred['user'], $cred['pwd'], $cred['db']); 

		// Check connection
		if ($conn->connect_error) die("Connection failed: " . $conn->connect_error); 
		// echo "Connected successfully<br>";	

		return $conn;
	}*/

	
		
	$conn->close();	 
?>

</div>
</body>
</html>