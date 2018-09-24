def is_leap_year(year):
	if (year % 4) == 0 and (year % 100) != 0 or (year % 400) == 0:
		print("{0} is leap year".format(year))
	else:
		print("{0} not is leap year".format(year))