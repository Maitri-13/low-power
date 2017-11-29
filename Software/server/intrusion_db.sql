
-- create the events DB
DROP DATABASE IF EXISTS events;
CREATE DATABASE events;
USE events;

-- create intrusion_events table
CREATE TABLE intrusion_events( node_id INT NOT NULL, location VARCHAR(100) NOT NULL DEFAULT 'Unknown', message VARCHAR(100) NOT NULL DEFAULT 'Unknown Message', unixtime TIMESTAMP NOT NULL ON UPDATE CURRENT_TIMESTAMP, t_idx INT NOT NULL AUTO_INCREMENT, primary key(t_idx));


