from persistence import *

import sys

def main(args : list):
    inputfilename : str = args[1]
    with open(inputfilename) as inputfile:
        for line in inputfile:
            splittedline : list[str] = line.strip().split(", ")
            #TODO: apply the action (and insert to the table) if possible
            quantity = int(splittedline[1])
            if (quantity>0):
                repo.Activities.insert(Activitie(*splittedline))
                repo.execute_command(f""" UPDATE Products SET quantity = {str(repo.Products.find(id=splittedline[0])[0].quantity + quantity)} WHERE id = {str(splittedline[0])}""")
            elif(quantity<0 and repo.Products.find(id=splittedline[0])[0].quantity>=-quantity):  
                repo.Activities.insert(Activitie(*splittedline))
                repo.execute_command(f""" UPDATE Products SET quantity = {str(repo.Products.find(id=splittedline[0])[0].quantity + quantity)} WHERE id = {str(splittedline[0])}""")

if __name__ == '__main__':
    main(sys.argv)