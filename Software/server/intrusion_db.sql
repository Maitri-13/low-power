
-- create the events DB
DROP DATABASE IF EXISTS events;
CREATE DATABASE events;
USE events;

-- create intrusion_events table
CREATE TABLE intrusion_events( t_idx INT NOT NULL AUTO_INCREMENT, node_id INT NOT NULL, location VARCHAR(100) NOT NULL DEFAULT 'Unknown', message VARCHAR(100) NOT NULL DEFAULT 'Unknown Message', audio_path VARCHAR(100) NOT NULL DEFAULT 'unknown.wav', image_path VARCHAR(100) NOT NULL DEFAULT 'unknown.jpg', unixtime TIMESTAMP NOT NULL, primary key(t_idx));


