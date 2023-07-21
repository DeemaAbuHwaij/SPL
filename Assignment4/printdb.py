from persistence import *

def main():
    print("Activities")
    activities = repo.Activities.find_all()
    for activity in activities:
        print((activity.product_id , activity.quantity , activity.activator_id , activity.date))
    print("Branches")
    branches = repo.Branches.find_all()
    for branch in branches:
        print((branch.id, str(branch.location), branch.number_of_employees))
    print("Employees")
    employees = repo.Employees.find_all()
    for employee in employees:
        print((employee.id, str(employee.name), employee.salary, employee.branche))
    print("Products")
    products = repo.Products.find_all()
    for product in products:
        print((product.id, str(product.description), product.price, product.quantity))
    print("Suppliers")
    suppliers = repo.Suppliers.find_all()
    for supplier in suppliers:
        print((supplier.id, str(supplier.name),str(supplier.contact_information)))
    print()
    print("Employees report")
    report = repo.execute_command('''SELECT Employees.name , Employees.salary , Branches.location ,
    IFNULL(SUM((Activities.quantity * (-1)) * Products.price), 0)
    FROM Employees 
    LEFT JOIN Activities ON Activities.activator_id=Employees.id 
    LEFT JOIN Products ON Products.id=Activities.product_id 
    LEFT JOIN Branches ON Branches.id=Employees.branche
    GROUP BY Name
    ORDER BY Name''')
    for item in report:
        print(*item)          
    print()
    report = repo.execute_command( '''SELECT Activities.date , Products.description , 
    Activities.quantity, Employees.name, Suppliers.name
    FROM Activities 
    LEFT JOIN Products ON Activities.product_id = products.id
    LEFT JOIN Employees ON Activities.activator_id = employees.id
    LEFT JOIN Suppliers ON Activities.activator_id = suppliers.id
    ORDER BY date''')
    print("Activities report")
    for item in report:
        print(item)

if __name__ == '__main__':
    main()