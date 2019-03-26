import matplotlib.pyplot as plt
import numpy as np

x = [1005.9253,1081.0363,1089.6973,1064.1083,1021.3287,
	1011.4868,1099.2767,1208.7189,1258.3221,1280.7617,
	1361.9904,1370.9137,1378.1311]
y = [1.53,20.87,23.03,15.97,5.43,2.91,31.12,54.04,67.54,73.61,93.92,96.34,98.07]

plt.scatter(x,y)
plt.ylabel("Temperature (C)")
plt.xlabel("Resistance (ohms)")
plt.title("Resistance vs Temperature for the PT1000");
coefs = np.polyfit(x,y,1)
xline = np.arange(1005,1378)
yline = []
label = "R(t) = " + str(coefs[0]) + "t" + "+" + str(coefs[1])
for x in xline:
	yline.append(coefs[0]*x + coefs[1])
bestfit, = plt.plot(xline,yline, color= 'red', label = label)
plt.legend(handles=[bestfit])
plt.show()