import java.io.BufferedWriter;
import java.io.FileWriter;

public class javaWrite
{
	public static void main(String[] args) throws Throwable
	{
		if(args.length != 2) {
			System.err.println("Usage: javaWrite filename data");
			System.exit(1);
		}
		BufferedWriter writer = new BufferedWriter(new FileWriter(args[0]));
		writer.write(args[1], 0, args[1].length());
		writer.close();
	}
}
