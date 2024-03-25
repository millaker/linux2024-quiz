import matplotlib.pyplot as plt
import numpy as np
import math

count = np.zeros(256, int)
with open('dist.txt', 'r') as file:
    for line in file:
        num, occur = line.split()
        num = int(num)
        occur = int(occur)
        count[num] += occur

accum_prob = np.zeros(256,float)
prob = np.zeros(256,float)
accum = 0
size = 100000000
for i in range(1,256):
    accum += count[i]
    prob[i] = count[i]/float(size)
    accum_prob[i] = accum/float(size)

x = np.arange(1, 255)

# Create bar plot
plt.figure(figsize=(10, 6))  # Set the figure size
plt.bar(x, count[1:255], color='skyblue')  # Create the bar plot with skyblue color
plt.xlabel('Element Index')  # Label for x-axis
plt.ylabel('Occurence')  # Label for y-axis
plt.title('Occurence Plot for 1 - 255')  # Title of the plot
plt.grid(True)  # Show grid lines
plt.tight_layout()  # Adjust layout for better spacing
plt.savefig('count.png')

plt.figure(figsize=(10, 6))  # Set the figure size
plt.plot(x, accum_prob[1:255])  
plt.xlabel('x')  # Label for x-axis
plt.ylabel('Probability')  # Label for y-axis
plt.title('CDF for Xorshift32 & 0xFF')  # Title of the plot
plt.tight_layout()  # Adjust layout for better spacing
plt.savefig('cdf.png')

# entropy
print('Calculate shannon entropy')
expect = 255 * -((1/255) * math.log(1/255))
entropy = 0
for i in range(1,256):
    entropy += prob[i] * math.log(prob[i])
entropy *= -1
print(f'Expected :{expect}')
print(f'Result   :{entropy}')
print(prob.sum())

