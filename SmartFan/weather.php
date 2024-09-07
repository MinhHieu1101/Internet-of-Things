<?php
	$host = 'localhost';
	$username = 'admin';
	$password = 'admin';
	$database = 'IoT_SU';
	$conn = new mysqli($host, $username, $password, $database);
	if ($conn->connect_error) {
		die("Connection failed: " . $conn->connect_error);
	}

	//create the "sensor_data" and "fan_threshold" table if they do not exist
	$createSensorData = "CREATE TABLE IF NOT EXISTS sensor_data (
						  id INT AUTO_INCREMENT PRIMARY KEY,
						  temperature DECIMAL(5, 2),
						  humidity DECIMAL(5, 2),
						  recorded_date DATETIME
						  );";

	$createFanThreshold = "CREATE TABLE IF NOT EXISTS fan_threshold (
							id INT AUTO_INCREMENT PRIMARY KEY,
							threshold DECIMAL(5, 2) NOT NULL
							);";

	$conn->query($createSensorData);
	$conn->query($createFanThreshold);

	//only store 1 fan threshold on the database
	$thresholdId = "SELECT 1 FROM fan_threshold WHERE id = 1";
	$thresholdIdResult = $conn->query($thresholdId);
	if ($thresholdIdResult->num_rows == 0) {
		$insertFanThreshold = "INSERT INTO fan_threshold (threshold) VALUES (26.84);";
		$conn->query($insertFanThreshold);
	}

	//fetch the current threshold from the table "fan_threshold"
	$thresholdQuery = "SELECT threshold FROM fan_threshold ORDER BY threshold DESC LIMIT 1";
	$thresholdResult = $conn->query($thresholdQuery);
	$currentThreshold = $thresholdResult->fetch_assoc();
	$fanThreshold = $currentThreshold['threshold'];
	
	//fetch the last 8 sensor data records from the table "sensor_data"
	$sensorDataQuery = "SELECT * FROM sensor_data ORDER BY recorded_date DESC LIMIT 8";
	$sensorDataResult = $conn->query($sensorDataQuery);
	$sensorDataArray = [];
	while ($row = $sensorDataResult->fetch_assoc()) {
	  array_push($sensorDataArray, $row);
	}
	
	//fetch the current temperature and humidity
	$currentQuery = "SELECT temperature, humidity FROM sensor_data ORDER BY recorded_date DESC LIMIT 1";
	$currentResult = $conn->query($currentQuery);
	$currentData = $currentResult->fetch_assoc();

	//fetch max temp, min temp, average temp and average humidity
	$statsQuery = "SELECT MAX(temperature) AS maxTemp, MIN(temperature) AS minTemp, AVG(temperature) AS avgTemp, AVG(humidity) AS avgHumidity FROM sensor_data";
	$statsResult = $conn->query($statsQuery);
	$statsData = $statsResult->fetch_assoc();
	
	$conn->close();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="description" content="Smart Home with IoT" />
    <meta name="keywords" content="HTML, CSS, PHP, MySQL, Javascript" />
    <meta name="author" content="A Normal Existence"  />
    <meta name="viewport" content="width=device-width, initial-scale=1.0"  />

	<link rel="preconnect" href="https://fonts.googleapis.com">
	<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
	<link href="https://fonts.googleapis.com/css2?family=Kalam:wght@300;400;700&display=swap" rel="stylesheet">
	<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.15.4/css/all.css">
	<link rel="stylesheet" href="style.css">
	
	<script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
	
    <title>Smart Home</title>
</head>

<body>
	<div class="frame">
		<div class="panel">
			<div>
				<div class="fan_threshold">
					Fan Temp Threshold: <?php echo $fanThreshold; ?>°C <br>
					<div>
						<form id="thresholdForm">
							<input type="text" id="threshold" name="threshold" placeholder="new threshold">
							<input type="submit" value="Change">
							<button type="button" onclick="refreshPage()">Data Refresh</button>
						</form>
					</div>
				</div>
				<div>
					<div class="current_date">
						<?php echo date("l") . " " . date("d/m/Y"); ?>
					</div>
					<table class="sensor_record">
					<tbody>
						<tr>
							<th>Date</th>
							<th>Temp</th>
							<th>RH</th>
						</tr>
						<?php foreach ($sensorDataArray as $sensorData): ?>
						<tr>
							<td><?php echo $sensorData['recorded_date']; ?></td>
							<td><?php echo $sensorData['temperature']; ?>°</td>
							<td><?php echo $sensorData['humidity']; ?>%</td>
						</tr>
						<?php endforeach; ?>
					</tbody>
					</table>
				</div>
				<div>
					<ul class="weather-details">
						<li><i class="fas fa-temperature-high"></i> Current temp: <?= isset($currentData['temperature']) ? htmlspecialchars($currentData['temperature']) . '°C' : 'N/A' ?></li>
						<li><i class="fas fa-water"></i> Current humidity: <?= isset($currentData['humidity']) ? htmlspecialchars($currentData['humidity']) . '%' : 'N/A' ?></li>
						<li><i class="fas fa-thermometer-full"></i> Max temp: <?= isset($statsData['maxTemp']) ? htmlspecialchars($statsData['maxTemp']) . '°C' : 'N/A' ?></li>
						<li><i class="fas fa-thermometer-empty"></i> Min temp: <?= isset($statsData['minTemp']) ? htmlspecialchars($statsData['minTemp']) . '°C' : 'N/A' ?></li>
						<li><i class="fas fa-thermometer-half"></i> Average temp: <?= isset($statsData['avgTemp']) ? round(htmlspecialchars($statsData['avgTemp']), 2) . '°C' : 'N/A' ?></li>
						<li><i class="fas fa-tint"></i> Average humidity: <?= isset($statsData['avgHumidity']) ? round(htmlspecialchars($statsData['avgHumidity']), 2) . '%' : 'N/A' ?></li>
					</ul>
				</div>
			</div>
		</div>
	</div>

<script>
	//send the new threshold value through an AJAX POST request to "update_threshold.php"
	$(document).ready(function() {
		$('#thresholdForm').submit(function(e) {
			e.preventDefault();
			var newThreshold = $('#threshold').val();
			$.ajax({
				type: 'POST',
				url: 'update_threshold.php',
				data: {threshold: newThreshold},
				success: function(response) {
					if (response.error) {
						alert(response.error);
					} else if (response.success) {
						alert(response.success);
					}
				},
				error: function(xhr, status, error) {
					alert("An error occurred: " + error);
				}
			});
		});
	});
	
	//reload the page to update the data from the tables
	function refreshPage(){
		window.location.reload();
	}
</script>

</body>
</html>

