
CREATE DATABASE dts CHARACTER SET utf8 COLLATE utf8_general_ci;

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
)ENGINE=MyISAM;

//~ gchar *sql_buf;
//~ sql_buf = g_strconcat(
              //~ "select ",
              //~ "  dep_busno, dep_dest, dep_standard, dep_time, dep_platform, ", 
              //~ "  (CASE ",
              //~ "    WHEN dep_depart = 0 THEN ' ' ",
              //~ "    WHEN dep_depart = 1 THEN 'เข้า' ",
              //~ "    WHEN dep_depart = 2 THEN 'ออก' ",
              //~ "  END) as dep_status , ",
              //~ "  dep_note "
              //~ "FROM ",
              //~ "  dts_depart ",
              //~ "WHERE ",
              //~ "  DATE_FORMAT(CONCAT(dep_date, ' ', dep_time), '%Y-%m-%d %T') > ",
              //~ "  DATE_FORMAT(now()-interval 30 minute, '%Y-%m-%d %T') AND ",
              //~ "  dep_date = DATE(curdate()) ",
              //~ "ORDER BY dep_date, dep_time ", NULL);
