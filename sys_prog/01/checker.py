import random
import argparse

maxint = 1 << 31

parser = argparse.ArgumentParser(description = "Check that a file contains "\
					       "not decreasing sequence of "\
					       "numbers")
parser.add_argument('-f', type=str, required=True, help="file name")
args = parser.parse_args()


f = open(args.f, 'r')
data = f.read()
f.close()

data = data.split()
previous_number = - (maxint-1)
print(previous_number)
for i in range(0, len(data)):
	try:
		v = int(data[i])
		if v < previous_number:
			print('Error on numbers {} {}'.format(previous_number, v))
			break
		prev_numbers = v
	except:
		pass

print('All is ok')