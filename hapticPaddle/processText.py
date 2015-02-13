f.open("values.txt","r+")

lines = f.readlines()

for l in lines:
	rawDeg = float(l[0:l.index(":")])
	flipNum = float(l[l.index(":"):l.index("\n")])

	deg = rawDeg*.6
	flipdeg = deg + 360*flipNum

	print flipdeg
