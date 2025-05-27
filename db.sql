CREATE DATABASE control_rfid;

USE control_rfid;

CREATE TABLE tarjetas (
    id INT AUTO_INCREMENT PRIMARY KEY,
    uid VARCHAR(20) NOT NULL UNIQUE,
    nombre VARCHAR(50),
    fecha_registro TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    autorizada BOOLEAN DEFAULT TRUE
);

INSERT INTO tarjetas (uid, nombre) VALUES ('A1B2C3D4', 'Tarjeta Maestra');