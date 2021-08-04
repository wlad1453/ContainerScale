<!DOCTYPE html>
<html>
<head>
<style>
body { background-color: #c4e3ed;
	font-family: arial;
	}
.sensData {
	table-layout: fixed;
	width: 740px;
	border-collapse: collapse;  
	white-space: nowrap;
}
.row-ID {width: 7%;}
.row-DateTime {width: 15%;}
.row-Data {width: 12%;}
.row-sb {width: 3%;
		border: none;}

table, td, th {
  border: 2px solid black;
  padding: 5px;
  align: center;
  text-align: center;
}

</style>
</head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<body>

	<div align="center"><h3>Container scale values</h3></div>
	<div align="center">	
		<label for="start"> Start date:</label>
		<input type="date" id="start" name="trip-start"
			   value="2020-11-01"  min="2018-01-01" max="2022-12-31">
			   
		<label for="stop">  End date:</label>
		<input type="date" id="stop" name="trip-stop"
			   value="2020-11-01"  min="2018-01-01" max="2022-12-31"><br><br>		   
	</div>	
	<div align="center">
		<table class="sensData" width="740">
			<tr>
			<th class="row-1 row-ID">No</th> <th class="row-1 row-DateTime">Time</th> <th class="row-1 row-DateTime">Date</th> 
			<th class="row-1 row-Data">Sens1</th> <th class="row-1 row-Data">Sens2</th> <th class="row-1 row-Data">Sens3</th> <th class="row-1 row-Data">Sens4</th>
			<th class="row-1 row-Data">Weight</th>
			<th class="row-1 row-sb">  </th>
			</tr>
		</table>
	<iframe src="Scale_Data.php" align="center" height="700" width="740" style="border:none; overflow: hidden; overflow-y:auto;">
	</div>
</body>
</html>