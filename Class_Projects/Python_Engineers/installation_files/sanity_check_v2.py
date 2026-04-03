import numpy as np
import matplotlib.pyplot as plt
import cv2
from PyQt5 import QtWidgets
import pyqtgraph as pg
import scipy

print("All packages imported successfully!")

# Simple plot test
x = np.linspace(0, 2*np.pi, 100)
y = np.sin(x)

plt.plot(x, y)
plt.title("Sanity Check Plot")
plt.show()