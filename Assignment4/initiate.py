from persistence import *

import sys
import os

def add_branche(splittedline):
    #[id , location , num of employees]
    branche_id, branche_location, branche_number_of_employees= splittedline
    branche = Branche(branche_id, branche_location, branche_number_of_employees)
    repo.Branches.insert(branche)

def add_supplier(splittedline):
    #[id , name , contact information]
    supplier_id, supplier_name, supplier_contact_information = splittedline
    supplier = Supplier(supplier_id, supplier_name, supplier_contact_information)
    repo.Suppliers.insert(supplier)

def add_product(splittedline):
    #[id , description , price , quantity]
    product_id, product_name, product_price, product_quantity = splittedline
    product = Product(product_id , product_name , product_price, product_quantity)
    repo.Products.insert(product)

def add_employee(splittedline):
    #[id , name , salary , baranche]
    employee_id, employee_name, employee_salary, employee_branche_id = splittedline
    employee = Employee(employee_id , employee_name , employee_salary, employee_branche_id)
    repo.Employees.insert(employee)

adders = {  "B": add_branche,
            "S": add_supplier,
            "P": add_product,
            "E": add_employee}

def main(args: list):
    inputfilename = args[1]
    # delete the database file if it exists
    repo._close()
    # uncomment if needed
    if os.path.isfile("bgumart.db"):
         os.remove("bgumart.db")
    repo.__init__()
    repo.create_tables()
    with open(inputfilename) as inputfile:
        for line in inputfile:
            splittedline : list[str] = line.strip().split(",")
            adders.get(splittedline[0])(splittedline[1:])

if __name__ == '__main__':
    main(sys.argv)