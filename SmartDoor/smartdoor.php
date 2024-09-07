<?php
$servername = "localhost";
$username = "admin";
$password = "admin";
$dbname = "assignment3_smarthome";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    if (isset($_POST['new_password'])) {
        $new_password = $_POST['new_password'];
        $conn->query("UPDATE DoorPassword SET password='$new_password' WHERE id=1");
    }

    if (isset($_POST['servo_position'])) {
        $servo_position = $_POST['servo_position'];
        $conn->query("UPDATE ServoControl SET servo_position='$servo_position' WHERE id=1");
    }
}

$result = $conn->query("SELECT * FROM AccessHistory ORDER BY timestamp DESC LIMIT 20");
$history = [];
if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $history[] = $row;
    }
}

$current_password = $conn->query("SELECT password FROM DoorPassword WHERE id=1")->fetch_assoc()['password'];
$servo_position = $conn->query("SELECT servo_position FROM ServoControl WHERE id=1")->fetch_assoc()['servo_position'];
?>

<!DOCTYPE html>
<html>
<head>
  <title>Smart Door Control</title>
  <link rel="stylesheet" type="text/css" href="style.css">
  <script type="text/javascript">
    function updateSliderValue(val) {
      document.getElementById('sliderValue').innerText = val;
    }
  </script>
</head>
<body>
	<div class="container">
  <h2>Smart Door Control</h2>
  <form method="post" action="">
    New Password: <input type="text" name="new_password" value="<?php echo $current_password; ?>"><br><br>
    Servo Position: <input type="range" name="servo_position" min="0" max="180" value="<?php echo $servo_position; ?>" oninput="updateSliderValue(this.value)">
	<span id="sliderValue"><?php echo $servo_position; ?></span><br><br>
	<button type="submit" class="button">Update</button>
  </form>
  <h2>Access History</h2>
  <table border="1">
    <tr>
      <th>ID</th>
      <th>Timestamp</th>
      <th>Door State</th>
      <th>Password Input</th>
    </tr>
    <?php foreach ($history as $entry) { ?>
    <tr>
      <td><?php echo $entry['id']; ?></td>
      <td><?php echo $entry['timestamp']; ?></td>
      <td><?php echo $entry['door_state']; ?></td>
      <td><?php echo $entry['password_input']; ?></td>
    </tr>
    <?php } ?>
  </table>

  <h2>Wrong Password Attempts</h2>
  <iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/2613709/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line"></iframe>
	</div>
</body>
</html>
