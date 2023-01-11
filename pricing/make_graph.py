import matplotlib.pyplot as plt
import pandas as pd

dir = '/home/suzuki/master/pricing/1/'

# 容量超過ふらぐ
df_over = pd.read_csv(dir + 'overTrans.csv')

# コスト合計のグラフを作る
fig_cost = plt.figure()
ax_cost = fig_cost.add_subplot(111)
df = pd.read_csv(dir + 'sumCostTrans.csv')
ax_cost.plot(df['step'], df['sumcost'])
# ax_cost.set_ylim(300, 350)
ax_cost.plot(df_over['step'], df_over['is_over'] * 300)
fig_cost.savefig(dir + 'sumCostTrans.png')

# fig_cost = plt.figure()
# ax_cost = fig_cost.add_subplot(111)
# ax_cost.plot(df['step'].iloc[50000:], df['sumcost'].iloc[50000:])
# # ax_cost.plot(df_over['step'], df_over['is_over'] * 30)
# fig_cost.savefig(dir + 'sumCostTransLatter.png')

# fig_cost = plt.figure()
# ax_cost = fig_cost.add_subplot(111)
# df = pd.read_csv(dir + 'sumCostTransWithout.csv')
# ax_cost.plot(df['step'], df['sumcost'])
# fig_cost.savefig(dir + 'sumCostTransWithout.png')


# 状態変化のグラフを作る
df_x = pd.read_csv(dir + 'xTrans.csv')
fig_x = plt.figure()
ax_x = fig_x.add_subplot(111)
for i in range(1, len(df_x.columns) - 1):
    ax_x.plot(df_x['step'], df_x.iloc[:, i])
# ax_x.plot(df_over['step'], df_over['is_over'] * 5)
ax_x.legend()
fig_x.savefig(dir + 'xTrans.png')


# コスト変化のグラフを作る
df_c = pd.read_csv(dir + 'cTrans.csv')
fig_c = plt.figure()
ax_c = fig_c.add_subplot(111)
for i in range(1, len(df_c.columns) - 1):
    ax_c.plot(df_c['step'], df_c.iloc[:, i])
# ax_x.plot(df_over['step'], df_over['is_over'] * 5)
ax_c.legend()
fig_c.savefig(dir + 'cTrans.png')

# df_x = pd.read_csv(dir + 'xTransWithout.csv')
# fig_x = plt.figure()
# ax_x = fig_x.add_subplot(111)
# for i in range(1, len(df_x.columns) - 1):
#     ax_x.plot(df_x['step'], df_x.iloc[:, i])
# ax_x.legend()
# fig_x.savefig(dir + 'xTransWithout.png')

# fig_x_with = plt.figure()
# ax_x_with = fig_x_with.add_subplot(111)
# for i in range(3, 6):
#     ax_x_with.plot(df_x_with['step'].iloc[EQ:], df_x_with.iloc[EQ:, i])
# ax_x_with.axvspan(START, END, color='gray', alpha=0.3)
# fig_x_with.savefig(dir + 'xTransWith_1.png')

# fig_x_with = plt.figure()
# ax_x_with = fig_x_with.add_subplot(111)
# for i in range(6, 8):
#     ax_x_with.plot(df_x_with['step'].iloc[EQ:], df_x_with.iloc[EQ:, i])
# ax_x_with.axvspan(START, END, color='gray', alpha=0.3)
# fig_x_with.savefig(dir + 'xTransWith_2.png')

# fig_x_with = plt.figure()
# ax_x_with = fig_x_with.add_subplot(111)
# for i in range(8, 11):
#     ax_x_with.plot(df_x_with['step'].iloc[EQ:], df_x_with.iloc[EQ:, i])
# ax_x_with.axvspan(START, END, color='gray', alpha=0.3)
# fig_x_with.savefig(dir + 'xTransWith_3.png')

# fig_x_out = plt.figure()
# ax_x_out = fig_x_out.add_subplot(111)
# df_x_out = pd.read_csv(dir + 'x_trans_out.csv')
# for i in range(1, len(df_x_out.columns) - 1):
#     ax_x_out.plot(df_x_out['step'], df_x_out.iloc[:, i])
# ax_x_out.legend()
# fig_x_out.savefig(dir + 'xTransOut.png')


# リンクフローと課金額のグラフを作る
# for suffix in choice:
#     df_user = pd.read_csv(dir + 'user_trans' + suffix)
#     df_price = pd.read_csv(dir + 'price_trans' + suffix)
#     for i in range(1, len(df_user.columns) - 1):
#         fig_link = plt.figure()
#         ax_user = fig_link.add_subplot(111)
#         ax_user.plot(df_user['step'].iloc[1:], df_user.iloc[1:, i], color='blue')
#         ax_price = ax_user.twinx()
#         ax_price.set_ylim(0, 2)
#         ax_price.plot(df_price['step'].iloc[1:], df_price.iloc[1:, i], color='orange')
#         ax_price.axvspan(START, END, color='gray', alpha=0.3)
#         if suffix == '_with.csv':
#             fig_link.savefig(dir + 'linkTransWith_' + str(i) + '.png')
#         else:
#             fig_link.savefig(dir + 'linkTransOut_' + str(i) + '.png')

# df_user = pd.read_csv(dir + 'user_trans_with.csv')
# df_price = pd.read_csv(dir + 'price_trans_with.csv')
# df_cost = pd.read_csv(dir + 'cost_trans_with.csv')
# for i in range(1, len(df_user.columns) - 1):
#     fig_link = plt.figure()
#     ax_user = fig_link.add_subplot(111)
#     ax_user.plot(df_user['step'].iloc[EQ:], df_user.iloc[EQ:, i], color='blue')
#     ax_price = ax_user.twinx()
#     # ax_price.set_ylim(1.42, 1.51)
#     ax_price.plot(df_price['step'].iloc[EQ:], df_price.iloc[EQ:, i], color='orange')

#     # ax_price.plot(df_cost['step'].iloc[EQ:], df_cost.iloc[EQ:, i], color='green')
#     ax_user.axvspan(START, END, color='gray', alpha=0.3)
#     fig_link.savefig(dir + 'linkTransWith_' + str(i) + '.png')


# # 空間的な同リンクでまとめてリンクフローを比較
# df_user = pd.read_csv(dir + 'user_trans_with.csv')

# fig_user = plt.figure()
# ax_user = fig_user.add_subplot(111)
# ax_user.plot(df_user['step'].iloc[EQ:], df_user.iloc[EQ:, [1, 5, 9]])
# ax_user.axvspan(START, END, color='gray', alpha=0.3)
# fig_user.savefig(dir + 'userTransWith_0.png')

# fig_user = plt.figure()
# ax_user = fig_user.add_subplot(111)
# ax_user.plot(df_user['step'].iloc[EQ:], df_user.iloc[EQ:, [2, 6, 10]])
# ax_user.axvspan(START, END, color='gray', alpha=0.3)
# fig_user.savefig(dir + 'userTransWith_1.png')

# fig_user = plt.figure()
# ax_user = fig_user.add_subplot(111)
# ax_user.plot(df_user['step'].iloc[EQ:], df_user.iloc[EQ:, [3, 7, 11]])
# ax_user.axvspan(START, END, color='gray', alpha=0.3)
# fig_user.savefig(dir + 'userTransWith_2.png')

# fig_user = plt.figure()
# ax_user = fig_user.add_subplot(111)
# ax_user.plot(df_user['step'].iloc[EQ:], df_user.iloc[EQ:, [4, 8, 12]])
# ax_user.axvspan(START, END, color='gray', alpha=0.3)
# fig_user.savefig(dir + 'userTransWith_3.png')


# # 鉄道会社の収入
# df_sumprice = pd.read_csv(dir + 'sum_price_trans_with.csv')

# fig_sumprice = plt.figure()
# ax_sumprice = fig_sumprice.add_subplot(111)
# ax_sumprice.plot(df_sumprice['step'].iloc[EQ:], df_sumprice['sumprice'].iloc[EQ:])
# ax_sumprice.axvspan(START, END, color='gray', alpha=0.3)
# fig_sumprice.savefig(dir + 'sumPriceWith.png')


# # social welfare
# fig_welfare = plt.figure()
# ax_welfare = fig_welfare.add_subplot(111)
# ax_welfare.plot(df_sumprice['step'].iloc[EQ:], df_sumprice['sumprice'].iloc[EQ:] - df['sumcost'].iloc[EQ:])
# ax_welfare.axvspan(START, END, color='gray', alpha=0.3)
# fig_welfare.savefig(dir + 'socialWelfareWith.png')