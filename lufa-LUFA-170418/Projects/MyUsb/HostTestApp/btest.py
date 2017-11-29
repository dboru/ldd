import random
from struct import *
from bitstring import BitArray
# msg=[];
msg=''
for i in range(8):
	if i==0 or  i>4:
		# msg.append(0)
		msg+=str(0)
		# msg.append(0)
	else:
		# msg.append(random.randint(0, 1))
		msg+=str(random.randint(0, 1))
		# msg.append(random.randint(0, 1))
print(msg)
print int(msg,2)
# bpack=pack('s',msg)
# print(bpack)

