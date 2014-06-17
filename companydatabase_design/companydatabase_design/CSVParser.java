import java.awt.List;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.Statement;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.LinkedList;

public class CSVParser {
	//Saved lists of entity for additional purpose if any
	LinkedList<Department> departmentlist = new LinkedList<Department>();
	LinkedList<Employee> employeelist = new LinkedList<Employee>();
	LinkedList<Manager> managerlist = new LinkedList<Manager>();
	LinkedList<Salary> salarylist = new LinkedList<Salary>();
	
	public static void main(String[] args) throws Exception
	{
		CSVParser parser = new CSVParser();
		parser.run();
	}
	public void run() throws Exception {
		String file = "companydatabase.csv";
		BufferedReader br = null;
		String line = "";
		String comma =",";
		String colon = ";";
		DateFormat formater = new SimpleDateFormat("mm/dd/yyyy");
		//Connection between MySQL and Java
		Class.forName("com.mysql.jdbc.Driver");
		Connection con = DriverManager.getConnection("jdbc:mysql://localhost:3306/","java","java");
		Statement stmt = (Statement) con.createStatement();
		//Clear out table before parsing in information
		//Remove these lines if adding information instead
		stmt.executeUpdate("TRUNCATE TABLE companydatabase.department");
		stmt.executeUpdate("TRUNCATE TABLE companydatabase.employee");
		stmt.executeUpdate("TRUNCATE TABLE companydatabase.employee_department");
		stmt.executeUpdate("TRUNCATE TABLE companydatabase.manager");
		stmt.executeUpdate("TRUNCATE TABLE companydatabase.salary");
		
		try {
			
			br = new BufferedReader(new FileReader(file));
			while((line = br.readLine()) !=null)
			{
				//use comma as separator
				String[] textline = line.split(comma);
				if (textline.length == 2){
					//Insert statement into department table
					String insert = "INSERT INTO companydatabase.department VALUES (" + textline[0] + 
							",'" + textline[1] +"')";
					departmentlist.add(new Department(Integer.parseInt(textline[0]),textline[1]));
					System.out.println(insert);
					stmt.executeUpdate(insert);
				}else if(textline.length == 4){
					//Insert statement into salary table
					String insert ="";
					Date startdate = formater.parse(textline[1]);
					java.sql.Date start = new java.sql.Date(startdate.getTime());
					if( textline[2].equals("null")){
						//handle cases where end date is null
						insert = "INSERT INTO companydatabase.salary VALUES ('" + textline[0] + textline[1] + 
								"','"+ textline[0] + 
								"','" + start +
								"'," + textline[2] +
								"," + textline[3] + ")";
						salarylist.add(new Salary(textline[0],startdate, Integer.parseInt(textline[3])));
					}else{
						Date enddate = formater.parse(textline[2]);
						java.sql.Date end = new java.sql.Date(enddate.getTime());
						insert = "INSERT INTO companydatabase.salary VALUES ('" + textline[0] + textline[1] + 
								"','" + textline[0] + 
								"','" + start +
								"','" + end +
								"'," + textline[3] + ")";
						salarylist.add(new Salary(textline[0],startdate,enddate, Integer.parseInt(textline[3])));
					}
					System.out.println(insert);
					stmt.executeUpdate(insert);
				}else if(textline.length == 8){
					// Insert into employee table
					String insert = "";
					Date startdate = formater.parse(textline[5]);					
					java.sql.Date start = new java.sql.Date(startdate.getTime());	
					String[] dep = textline[7].split(colon);
					LinkedList<Integer> newdeplist = new LinkedList<Integer>();
					for(int i = 0; i < dep.length;i++){
						//Handle multiple departments
						String insertdep = "INSERT INTO companydatabase.employee_department VALUES ('" + textline[1] + dep[i] + 
								"','" + textline[1] + 
								"'," + Integer.parseInt(dep[i]) + ")";
						System.out.println(insertdep);
						stmt.executeUpdate(insertdep);
						newdeplist.add(new Integer(Integer.parseInt(dep[i])));
					}
					if(textline[6].equals("null")){
						//Handle cases where termination date is null
						insert = "INSERT INTO companydatabase.employee VALUES ('" +
								textline[0] + "','" + 
								textline[1] + "','" + 
								textline[2] + "','" + 
								textline[3] + "','" + 
								textline[4] + "','" +
								start + "'," +
								textline[6] + ")";
						employeelist.add(new Employee(textline[0],textline[1],textline[2],textline[3],textline[4],
								startdate,newdeplist));
					}else{
						Date termdate = formater.parse(textline[6]);
						java.sql.Date end = new java.sql.Date(termdate.getTime());
						insert = "INSERT INTO companydatabase.employee VALUES ('" +
								textline[0] + "','" + 
								textline[1] + "','" + 
								textline[2] + "','" + 
								textline[3] + "','" + 
								textline[4] + "','" +
								start + "','" +
								end + "')";
						employeelist.add(new Employee(textline[0],textline[1],textline[2],textline[3],textline[4],
								startdate,termdate,newdeplist));
					}
					System.out.println(insert);
					stmt.executeUpdate(insert);
				}else if(textline.length == 10){
					//Insert into manager table
					String insertem = "";
					String insertma = "";
					Date startdate = formater.parse(textline[5]);	
					Date managerdate = formater.parse(textline[9]); 
					java.sql.Date start = new java.sql.Date(startdate.getTime());
					java.sql.Date mngdate = new java.sql.Date(managerdate.getTime());
					String[] dep = textline[7].split(colon);
					LinkedList<Integer> newdeplist = new LinkedList<Integer>();
					for(int i = 0; i < dep.length;i++){
						//Handle multiple departments
						String insertdep = "INSERT INTO companydatabase.employee_department VALUES ('" + textline[1] + dep[i] + 
						"','" + textline[1] +
						"'," + Integer.parseInt(dep[i]) + ")";
						System.out.println(insertdep);
						stmt.executeUpdate(insertdep);
						newdeplist.add(new Integer(Integer.parseInt(dep[i])));
					}
					if(textline[6].equals("null")){
						//Handle cases where termination date is null
						insertem = "INSERT INTO companydatabase.employee VALUES ('" +
								textline[0] + "','" + //Employee type
								textline[1] + "','" + //Employee id
								textline[2] + "','" + //First name
								textline[3] + "','" + //Last name
								textline[4] + "','" + //Gender
								start + "'," +        //Hire date
								textline[6] +   //Termination date
								")";
						insertma = "INSERT INTO companydatabase.manager VALUES ('" +
								textline[1] + "'," +
								textline[8] + ",'" +  //Annual bonus
								mngdate + "')";       //Manager start date
						managerlist.add(new Manager(textline[0],textline[1],textline[2],textline[3],textline[4],
								startdate,newdeplist,Integer.parseInt(textline[8]),mngdate));
					}else{
						Date termdate = formater.parse(textline[6]);
						java.sql.Date end = new java.sql.Date(termdate.getTime());
						insertem = "INSERT INTO companydatabase.employee VALUES ('" +
								textline[0] + "','" + 
								textline[1] + "','" + 
								textline[2] + "','" + 
								textline[3] + "','" + 
								textline[4] + "','" +
								start + "','" +
								end +
								"')";
						insertma = "INSERT INTO companydatabase.manager VALUES ('" +
								textline[1] + "'," +
								textline[8] + ",'" +  //Annual bonus
								mngdate + "')";       //Manager start date
						managerlist.add(new Manager(textline[0],textline[1],textline[2],textline[3],textline[4],
								startdate,termdate,newdeplist,Integer.parseInt(textline[8]),mngdate));
					}
					System.out.println(insertem);
					System.out.println(insertma);
					stmt.executeUpdate(insertem);
					stmt.executeUpdate(insertma);
					
				}
			}
		}catch(FileNotFoundException e){
			e.printStackTrace();
		}catch(IOException e){
			e.printStackTrace();
		}finally{
			if(br != null){
				try{
					br.close();
				}catch(IOException e){
					e.printStackTrace();
				}
			}
		}
	}
	public void execute(){
		
	}
}
//Department class used by run() to store a list of department objects
class Department{
	protected int department_number;
	protected String department_name;
	public Department(){
		this.department_number = -1;
		this.department_name = "Null";
	}
	public Department(int dep_num, String dep_nam){
		this.department_number = dep_num;
		this.department_name = dep_nam;
	}
	public int getDepartmentNumber(){
		return this.department_number;
	}
	public void setDepartmentNumber(int dep_num){
		this.department_number = dep_num;
	}
	public String getDepartmentName(){
		return this.department_name;
	}
	public void setDepartmentName(String dep_nam){
		this.department_name = dep_nam;
	}
}
//Employee class used by run() to store a list of employee (not manager)objects
class Employee{
	protected String employee_type,employee_id,first_name,last_name,gender;
	protected Date hire_date, termination_date;
	protected LinkedList<Integer> departmentlist = new LinkedList<Integer>();
	public Employee(){
	;
	}
	public Employee(String em_type, String em_id, String f_nam, String l_nam, String gen,
			Date h_date, Date t_date, LinkedList<Integer> dep_list){
		this.employee_type = em_type;
		this.employee_id = em_id;
		this.first_name = f_nam;
		this.last_name = l_nam;
		this.gender = gen;
		this.hire_date = h_date;
		this.termination_date = t_date;
		this.departmentlist = dep_list;
	}
	public Employee(String em_type, String em_id, String f_nam, String l_nam, String gen,
			Date h_date, LinkedList<Integer> dep_list){
		this.employee_type = em_type;
		this.employee_id = em_id;
		this.first_name = f_nam;
		this.last_name = l_nam;
		this.gender = gen;
		this.hire_date = h_date;
		this.departmentlist = dep_list;
	}
	public String getEmployeeType(){
		return this.employee_type;
	}
	public void setEmployeeType(String em_type){
		this.employee_type = em_type;
	}
	public String getEmployeeId(){
		return this.employee_id;
	}
	public void setEmployeeId(String em_id){
		this.employee_id = em_id;
	}
	public String getFirstName(){
		return this.first_name;
	}
	public void setFirstName(String f_name){
		this.first_name = f_name;
	}
	public String getLastName(){
		return this.last_name;
	}
	public void setLastName(String l_name){
		this.last_name = l_name;
	}
	public String getGender(){
		return this.gender;
	}
	public void setGender(String gen){
		this.gender = gen;
	}
	public Date getHireDate(){
		return this.hire_date;
	}
	public void setHireDate(Date h_date){
		this.hire_date = h_date;
	}
	public Date getTerminationDate(){
		return this.termination_date;
	}
	public void setTerminationDate(Date t_date){
		this.termination_date = t_date;
	}
	public LinkedList<Integer> getDepartmentList(){
		return this.departmentlist;
	}
}
//Manager class used by run() to store a list of managers
class Manager extends Employee{
	protected int annual_bonus;
	protected Date manager_start_date;
	public Manager(){
		;
	}
	public Manager(String em_type, String em_id, String f_nam, String l_nam, String gen,
			Date h_date, Date t_date,LinkedList<Integer> dep_list, int an_bonus, Date man_st_date){
		this.employee_type = em_type;
		this.employee_id = em_id;
		this.first_name = f_nam;
		this.last_name = l_nam;
		this.gender = gen;
		this.hire_date = h_date;
		this.termination_date = t_date;
		this.annual_bonus = an_bonus;
		this.manager_start_date = man_st_date;
		this.departmentlist = dep_list;
	}
	public Manager(String em_type, String em_id, String f_nam, String l_nam, String gen,
			Date h_date,LinkedList<Integer> dep_list, int an_bonus, Date man_st_date){
		this.employee_type = em_type;
		this.employee_id = em_id;
		this.first_name = f_nam;
		this.last_name = l_nam;
		this.gender = gen;
		this.hire_date = h_date;
		this.annual_bonus = an_bonus;
		this.manager_start_date = man_st_date;
		this.departmentlist = dep_list;
	}
	public int getAnnualBonus(){
		return this.annual_bonus;
	}
	public void setAnnualBonus(int an_bonus){
		this.annual_bonus = an_bonus;
	}
	public Date getManagerStartDate(){
		return this.manager_start_date;
	}
	public void setManagerStartDate(Date man_st_date){
		this.manager_start_date = man_st_date;
	}
}
//Salary class used by run() to store a list of all salary for employees and managers
class Salary{
	protected String employee_id;
	protected Date start_date, end_date;
	protected int salary;
	public Salary(){
		;
	}
	public Salary(String em_id, Date s_date, Date e_date, int s){
		this.employee_id = em_id;
		this.start_date = s_date;
		this.end_date = e_date;
		this.salary = s;
	}
	public Salary(String em_id, Date s_date, int s){
		this.employee_id = em_id;
		this.start_date = s_date;
		this.salary = s;
	}
	public String getEnployeeID(){
		return this.employee_id;
	}
	public void setEmployeeID(String em){
		this.employee_id = em;
	}
	public Date getStartDate(){
		return this.start_date;
	}
	public void setStartDate(Date s_date){
		this.start_date = s_date;
	}
	public Date getEndDate(){
		return this.end_date;
	}
	public void setEndDate(Date e_date){
		this.end_date = e_date;
	}
	public int getSalary(){
		return this.salary;
	}
	public void setSalary(int s){
		this.salary = s;
	}
}