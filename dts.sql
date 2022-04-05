USE dts;
DROP TABLE if exists dts_depart;
CREATE TABLE dts_depart
(
  id			int		NOT NULL PRIMARY KEY, 
  dep_time		varchar(5), 
  dep_dest		varchar(20), 
  dep_busno		varchar(7), 
  dep_standard	varchar (10), 
  dep_platform	varchar (10), 
  dep_note		varchar(20), 
  dep_depart	smallint,
  dep_datetime	datetime
);
