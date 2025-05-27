<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE');
header('Access-Control-Allow-Headers: Content-Type');

$servername = "localhost";
$username = "root"; 
$password = ""; 
$dbname = "control_rfid";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die(json_encode(['error' => 'Connection failed: ' . $conn->connect_error]));
}

$method = $_SERVER['REQUEST_METHOD'];

switch ($method) {
    case 'GET':
        if (isset($_GET['uid'])) {
            // Consultar tarjeta específica
            $uid = $_GET['uid'];
            $stmt = $conn->prepare("SELECT * FROM tarjetas WHERE uid = ?");
            $stmt->bind_param("s", $uid);
            $stmt->execute();
            $result = $stmt->get_result();
            
            if ($result->num_rows > 0) {
                echo json_encode($result->fetch_assoc());
            } else {
                echo json_encode(['error' => 'Tarjeta no encontrada']);
            }
        } else {
            // Listar todas las tarjetas
            $result = $conn->query("SELECT * FROM tarjetas");
            $tarjetas = [];
            while ($row = $result->fetch_assoc()) {
                $tarjetas[] = $row;
            }
            echo json_encode($tarjetas);
        }
        break;
        
    case 'POST':
        $data = json_decode(file_get_contents('php://input'), true);
        if (isset($data['uid'])) {
            $uid = $data['uid'];
            $nombre = isset($data['nombre']) ? $data['nombre'] : null;
            
            // Verificar si ya existe
            $check = $conn->prepare("SELECT id FROM tarjetas WHERE uid = ?");
            $check->bind_param("s", $uid);
            $check->execute();
            
            if ($check->get_result()->num_rows > 0) {
                echo json_encode(['error' => 'La tarjeta ya existe']);
            } else {
                $stmt = $conn->prepare("INSERT INTO tarjetas (uid, nombre) VALUES (?, ?)");
                $stmt->bind_param("ss", $uid, $nombre);
                if ($stmt->execute()) {
                    echo json_encode(['success' => 'Tarjeta registrada']);
                } else {
                    echo json_encode(['error' => 'Error al registrar']);
                }
            }
        } else {
            echo json_encode(['error' => 'UID requerido']);
        }
        break;
        
    case 'DELETE':
        $data = json_decode(file_get_contents('php://input'), true);
        if (isset($data['uid'])) {
            $uid = $data['uid'];
            $stmt = $conn->prepare("DELETE FROM tarjetas WHERE uid = ?");
            $stmt->bind_param("s", $uid);
            if ($stmt->execute()) {
                echo json_encode(['success' => 'Tarjeta eliminada']);
            } else {
                echo json_encode(['error' => 'Error al eliminar']);
            }
        } else {
            echo json_encode(['error' => 'UID requerido']);
        }
        break;
        
    default:
        echo json_encode(['error' => 'Método no soportado']);
        break;
}

$conn->close();
?>