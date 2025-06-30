<?php
$host = "localhost";     // atau 127.0.0.1
$user = "root";          // user default XAMPP
$pass = "";              
$db   = "iotdb";         

$koneksi = new mysqli($host, $user, $pass, $db);

// Cek koneksi
if ($koneksi->connect_error) {
    die("Koneksi database gagal: " . $koneksi->connect_error);
}
?>
