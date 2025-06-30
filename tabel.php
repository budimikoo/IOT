<?php
header('Content-Type: application/json');
$conn = new mysqli("localhost", "root", "", "iotdb");

if ($conn->connect_error) {
    die(json_encode(["status" => "error", "message" => "Koneksi gagal"]));
}

$result = $conn->query("SELECT * FROM sensor_data ORDER BY waktu DESC LIMIT 20");

$data = [];
while ($row = $result->fetch_assoc()) {
    $data[] = $row;
}

echo json_encode($data);
?>
