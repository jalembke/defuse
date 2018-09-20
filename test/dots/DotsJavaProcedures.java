
import java.sql.*;
import java.io.*;

import java.io.IOException;

public class DotsJavaProcedures
{ private static final String copyrightNotice = "(c) Copyright IBM Corp. 2001. All Rights Reserved.";

	public static void updateProc(int value, String condition)
		throws SQLException {

			Connection conn = DriverManager.getConnection("jdbc:default:connection");
			PreparedStatement ps = conn.prepareStatement(  "update basic2 set rnd_Integer = ? where basic2.ID_2 like ?");
			ps.setInt(1,value);
			ps.setString(2,condition);

			ps.executeUpdate();
			conn.close();
		}

	public static void queryProc(int condition,int[] count)
		throws SQLException {
			Connection conn = DriverManager.getConnection("jdbc:default:connection");
			PreparedStatement ps = conn.prepareStatement( "SELECT count(*) FROM BASIC1, BASIC2, BASIC3 WHERE BASIC1.ID_1 = BASIC3.ID_1 and BASIC2.ID_2 = BASIC3.ID_2 and  basic3.rnd_int > ?");
  			ps.setInt(1,condition);
			ResultSet rs = ps.executeQuery();
			if(rs.next())
				count[0] = rs.getInt(1);
			conn.close();
		}

	}