1. How to interpret the CSV:

There are 4 types of rows:
Department - 2 columns (department number, department name)

Employee - 8 columns (employee type, employee id, first name, last name, gender, hire date, termination date*, department number(s)**)

Manager - 10 columns (employee type, employee id, first name, last name, gender, hire date, termination date, department number(s)*, annual bonus, manager start date)

Salary*** - 4 columns (employee id, start date, end date, salary)

Special things to note:
*if an employee is currently still hired, their termination date is null
**an employee can be in 1 or more departments for parsing purposes, each department number is separated by a “;”
***an employee’s salary can change multiple times each will have a start or end date, unless the salary is their current salary in which case, the end date is null
*Managers are also employees*

2. Queries to get the following information:

a.Get the names and departments of all managers
b.Get all employees who are “MALE”
c.Get the name of the department(s) that currently does not have manager (the       manager has a termination date)
d.Get the salary, first name, last name, and gender of the employee(s) that   is/are currently hired with the lowest salary
e.Get the name and salary of the highest paid employee in “ENGINEERING” that is   not a manager

