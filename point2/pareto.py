import pandas as pd
import matplotlib.pyplot as plt
from adjustText import adjust_text

DIR = '/home/suzuki/master/point2/'
FONTSIZE = 15

s_store = []
s_obj = []
s_rail = []
for i in range(21, 41):
    CUR = DIR + str(i) + '/'
    df_store = pd.read_csv(CUR + 'storeRevenueTrans.csv')
    df_obj = pd.read_csv(CUR + 'sumCostTrans.csv')
    df_rail = pd.read_csv(CUR + 'railCostTrans.csv')

    s_store.append(df_store['storeRevenue'].mean())
    s_obj.append(-df_obj['sumCost'].iloc[len(df_obj) - 1])
    s_rail.append(df_rail['railCost'].iloc[len(df_rail) - 1])

print(len([i for i in range(11)] + [(i + 2) * 10 for i in range(9)]))
print(len(s_store))

df_pareto = pd.DataFrame(
    data={
        'm_max': [i for i in range(11)] + [(i + 2) * 10 for i in range(9)],
        'revenue': s_store,
        'objective': s_obj,
        'rail': s_rail,
    }
)

print(df_pareto)

fig_obj = plt.figure()
ax_obj = fig_obj.add_subplot(111)
ax_obj.scatter(df_pareto['revenue'], df_pareto['objective'])
text_obj = []
for i in range(len(df_pareto)):
    text_obj.append(ax_obj.text(df_pareto['revenue'].iloc[i], df_pareto['objective'].iloc[i], df_pareto['m_max'].iloc[i], fontsize=FONTSIZE-1))
ax_obj.set_xlabel('Revenue from stores of rail company', fontsize=FONTSIZE)
ax_obj.set_ylabel('Value of objective function', fontsize=FONTSIZE)
adjust_text(text_obj, arrowprops=dict(arrowstyle='->', color='red'))
ax_obj.tick_params(labelsize=FONTSIZE-1)
fig_obj.subplots_adjust(left=0.16, bottom=0.14)
fig_obj.savefig(DIR + 'paretoObj.png')

fig_rail = plt.figure()
ax_rail = fig_rail.add_subplot(111)
ax_rail.scatter(df_pareto['revenue'], df_pareto['rail'])
text_rail = []
for i in range(len(df_pareto)):
    text_rail.append(ax_rail.text(df_pareto['revenue'].iloc[i], df_pareto['rail'].iloc[i], df_pareto['m_max'].iloc[i], fontsize=FONTSIZE-1))
ax_rail.set_xlabel('Revenue from stores of rail company', fontsize=FONTSIZE)
ax_rail.set_ylabel('Sum of congestion cost', fontsize=FONTSIZE)
ax_rail.tick_params(labelsize=FONTSIZE-1)
adjust_text(text_rail, arrowprops=dict(arrowstyle='->', color='red'))
fig_rail.subplots_adjust(left=0.16, bottom=0.14)
fig_rail.savefig(DIR + 'paratoRail.png')