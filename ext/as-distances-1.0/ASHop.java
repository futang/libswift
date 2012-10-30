package se.sics.asdistances;
import java.io.*;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.zip.*;

public class ASHop{

    public static void main(String args[])
    {
        ASDistances distances = ASDistances.getInstance();
        
        // Check how many arguments were passed in
        if(args == null || args.length == 0)
        {
            System.out.println("Pass two IP addresses as argument");
            System.exit(0);
        }

        byte d = distances.getDistance(args[0], args[1]);
        System.out.println(d);
    }
}
