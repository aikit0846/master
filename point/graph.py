import pandas as pd
import math
import matplotlib.pyplot as plt


def base_func1(x):
    return 10000


def base_func2(theta3, theta4, x):
    return 1000 * math.tanh(0.00001 * theta3 * x - 0.00001 * 100000 * theta4)


df = pd.read_csv('/home/suzuki/master/point/value.csv')
df_param = pd.read_csv('/home/suzuki/master/point/params.csv')
df_reve = pd.read_csv('/home/suzuki/master/point/revenue.csv')
print(df_param)

fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(df['points'], df['value'])


base_param1 = df_param.iloc[0, 0]
base_param2 = df_param.iloc[1, 0]
base_param3 = df_param.iloc[2, 0]
base_param4 = df_param.iloc[3, 0]
x = [i for i in range(len(df) - 1)]
y = [base_param1 * base_func1(i) + base_param2 * base_func2(base_param3, base_param4, i) for i in range(len(df) - 1)]
ax.plot(x, y)


fig.savefig('/home/suzuki/master/point/valueFunc.png')


fig2 = plt.figure()
ax2 = fig2.add_subplot(111)
ax2.plot(df_reve['Points'], df_reve['Revenue'])
fig2.savefig('/home/suzuki/master/point/revenue.png')