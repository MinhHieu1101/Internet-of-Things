<?php
	 //indicate the JSON response type in order to send error and success messages
	header('Content-Type: application/json');

	$host = 'localhost';
	$username = 'admin';
	$password = 'admin';
	$database = 'IoT_SU';
	$conn = new mysqli($host, $username, $password, $database);
	if ($conn->connect_error) {
		die(json_encode(['error' => "Connection failed: " . $conn->connect_error]));
	}

	if ($_SERVER["REQUEST_METHOD"] == "POST") {
		//verify the new threshold to ensure that it's a float number and reasonable for a temperature
		$newThreshold = filter_input(INPUT_POST, 'threshold', FILTER_VALIDATE_FLOAT);
		if ($newThreshold === false) {
			echo json_encode(['error' => "Threshold must be a number."]);
		} elseif ($newThreshold < 0.00 || $newThreshold > 100.00) {
			echo json_encode(['error' => "Threshold must be between 0.00 and 100.00."]);
		} else {
			$newThreshold = $conn->real_escape_string((string)$newThreshold);
			
			//update the new threshold after validation
			$updateQuery = "UPDATE fan_threshold SET threshold = $newThreshold";
			if ($conn->query($updateQuery) === TRUE) {
				echo json_encode(['success' => "New threshold updated successfully: " . $newThreshold]);
			} else {
				echo json_encode(['error' => "There is an error with updating the new threshold: " . $conn->error]);
			}
		}
		$conn->close();
	}
?>

