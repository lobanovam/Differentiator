import numpy as np
import matplotlib.pyplot as plt
x = np.linspace(-10.000000, 10.000000, 200)
y = np.cos(x**(3.0))
+(np.sin(15.0*(x))
**(2.0))
plt.figure(figsize=(8,6), dpi=100)
plt.grid(True, linestyle="--")
plt.axis([-10.000000, 10.000000, np.min(y)-0.25, np.max(y)+0.25])
plt.plot(x, y, "-m",linewidth=1)
plt.savefig('graphFunc.png')