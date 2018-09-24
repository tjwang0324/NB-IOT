def fab(n):
	if n == 1:
		return 0
	if n == 2:
		return 1
	if n > 2:
		return fab(n-1) + fab(n-2)

def printfablist(n):
	for i in range(1, n+1):
		print(fab(i),end = ' ')
	print('')