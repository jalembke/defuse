import java.io.FileOutputStream;
import java.io.BufferedOutputStream;

public class javaOutputStream
{
	public static void main(String[] args) throws Throwable
	{
		if(args.length != 2) {
			System.err.println("Usage: javaOutputStream file data");
			System.exit(1);
		}
		BufferedOutputStream stream = new BufferedOutputStream(new FileOutputStream(args[0]));
		stream.write(args[1].getBytes());
		stream.close();
	}
}
