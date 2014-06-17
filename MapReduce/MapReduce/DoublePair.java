/*
 * CS 61C Fall 2013 Project 1
 *
 * DoublePair.java is a class which stores two doubles and 
 * implements the Writable interface. It can be used as a 
 * custom value for Hadoop. To use this as a key, you can
 * choose to implement the WritableComparable interface,
 * although that is not necessary for credit.
 */

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

import org.apache.hadoop.io.Writable;

public class DoublePair implements Writable {
    // Declare any variables here
    double d1,d2;
    /**
     * Constructs a DoublePair with both doubles set to zero.
     */
    public DoublePair() {
        d1 = 0;
        d2 = 0;
    }

    /**
     * Constructs a DoublePair containing double1 and double2.
     */ 
    public DoublePair(double double1, double double2) {
     d1 = double1;
	 d2 = double2;
    }

    /**
     * Returns the value of the first double.
     */
    public double getDouble1() {
        return d1;
    }

    /**
     * Returns the value of the second double.
     */
    public double getDouble2() {
        return d2;
    }

    /**
     * Sets the first double to val.
     */
    public void setDouble1(double val) {
	 this.d1 = val;
    }

    /**
     * Sets the second double to val.
     */
    public void setDouble2(double val) {
	 this.d2 = val;
    }

    /**
     * write() is required for implementing Writable.
     */
    public void write(DataOutput out) throws IOException {
	 out.writeDouble(d1);
	 out.writeDouble(d2);
    }

    /**
     * readFields() is required for implementing Writable.
     */
    public void readFields(DataInput in) throws IOException {
	 d1 = in.readDouble();
	 d2 = in.readDouble();
    }
    
    public static void main(String[] args){
        DoublePair pair1 = new DoublePair();
	 DoublePair pair2 = new DoublePair(12.5,55.3);
	 System.out.println("Initial values of first pair: " + pair1.getDouble1() + "  " + pair1.getDouble2());
	 System.out.println("Initial values of second pair: " + pair2.getDouble1() + "  " + pair2.getDouble2());
	 pair1.setDouble1(10.0);
	 pair1.setDouble2(10.0);
	 pair2.setDouble1(20.0);
	 pair2.setDouble2(20.0);
	 System.out.println("After setting, values of first pair: " + pair1.getDouble1() + "  " + pair1.getDouble2());
	 System.out.println("After setting, values of second pair: " + pair2.getDouble2() + "  " + pair2.getDouble2());

    }
}
