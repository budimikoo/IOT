<?php
require("phpMQTT.php");
use Bluerhinos\phpMQTT;

$server = "mqtt.revolusi-it.com";
$port = 1883;
$username = "usm";
$password = "usmjaya1";
$client_id = "sub_G231220128";

$mqtt = new phpMQTT($server, $port, $client_id);

if (!$mqtt->connect(true, NULL, $username, $password)) {
    exit(1);
}

$topic = "iot/G.231.22.0128";
$mqtt->subscribe([$topic => ["qos" => 0, "function" => "procmsg"]]);

while ($mqtt->proc()) {}

$mqtt->close();

function procmsg($topic, $msg){
    echo "Data diterima: $msg\n";

    $data = json_decode($msg, true);

    $conn = new mysqli("localhost", "root", "", "iotdb");
    if ($conn->connect_error) {
        die("Koneksi gagal: " . $conn->connect_error);
    }

    $sql = "INSERT INTO sensor_data (suhuDHT, suhuLM35, kelembapan, led_level)
            VALUES ('{$data['suhuDHT']}', '{$data['suhuLM35']}', '{$data['kelembaban']}', '{$data['LED']}')";
    if ($conn->query($sql) === TRUE) {
        echo "Data tersimpan!\n";
    } else {
        echo "Gagal simpan: " . $conn->error . "\n";
    }

    $conn->close();
}
