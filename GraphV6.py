#Here, the average data is shown, but from the last 50 scores(still updating) - better, cuz without dump for()
#Also median


import statistics as st
import numpy as np
import random
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

x_data = []
y_data = []

x2_data = []
y2_data = []

x3_data = []
y3_data = []

frame = np.arange(0, 100, 0.1)

lastFrames = []

sum = 0

fig, ax = plt.subplots()
ax.set_xlim(0, 40)
ax.set_ylim(0, 105)

line, = ax.plot(0, 0, linewidth = 0.5, label="Data")
line2, = ax.plot(0, 0, color='lime', label="Average Data")
line3, = ax.plot(0, 0, color='r', label="Median Data")

fig.canvas.manager.set_window_title('Sheesh okno')

plt.xlabel('X - Axis [sec]')
plt.ylabel('Y - Axis [data]')

def animation_frame(i):
    x_data.append(i)
    y_data.append(random.randint(1,100))

    line.set_xdata(x_data)
    line.set_ydata(y_data)
    
    #print("Y: " + str(y2_data) + " X: " + str(x2_data))
    lastFrames.append(y_data[len(y_data) - 1])
    
        
    
    return line, 

def Srednia(i):
    print("Dlugosc listy - zapisane wyniki: " + str(len(lastFrames)))
    if len(y2_data) == 0:
        y2_data.append(50)
        x2_data.append(0)
        line2.set_ydata(y2_data)
        line2.set_xdata(x2_data)    
    
    if len(lastFrames) >= 5:
        global sum
        if len(lastFrames) == 5:
            for x in range(len(lastFrames)):
                sum += lastFrames[x]
        elif len(lastFrames) > 5 and len(lastFrames) < 50:
            sum += lastFrames[-1]
        
        srednia = sum/len(lastFrames)
        
        print("Srednia: ")
        print(srednia)
        
        x2_data.append(i)
        y2_data.append(srednia)

        
        line2.set_xdata(x2_data)
        line2.set_ydata(y2_data)

        
        if len(lastFrames) >= 50:
            sum += lastFrames[-1]
            sum -= lastFrames[0]
            del lastFrames[0]
def Median(i):
    
    if len(y3_data) == 0:
        y3_data.append(50)
        x3_data.append(0)
        line3.set_ydata(y3_data)
        line3.set_xdata(x3_data)   
        
    if len(lastFrames) >= 5:
        y3_data.append(st.median(lastFrames))
        x3_data.append(i)
        
        line3.set_xdata(x3_data)
        line3.set_ydata(y3_data)    
      
        

            
    

animation = FuncAnimation(fig, func=animation_frame, frames=np.arange(0, 200, 0.1), interval=100) #Data
animation2 = FuncAnimation(fig, func=Srednia, frames=np.arange(0, 200, 0.1), interval=100) #Average
animation3 = FuncAnimation(fig, func=Median, frames=np.arange(0, 200, 0.1), interval=100) #Median

plt.legend()
plt.show()