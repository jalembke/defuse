connect 'jdbc:derby:TESTDB;user=u1;password=p1';
 CREATE PROCEDURE UPDATE_PROC(IN VALUE INTEGER, IN CONDITION CHAR(15))
 	LANGUAGE JAVA
 	PARAMETER STYLE JAVA
 	MODIFIES SQL DATA
 	EXTERNAL NAME 'DotsJavaProcedures.updateProc';
 
 CREATE PROCEDURE QUERY_PROC(IN CONDITION INTEGER, OUT c INTEGER)
 	LANGUAGE JAVA
 	PARAMETER STYLE JAVA
 	READS SQL DATA
 	EXTERNAL NAME 'DotsJavaProcedures.queryProc';

connect 'jdbc:derby:TESTDB;shutdown=true';
