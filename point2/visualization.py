import matplotlib.pyplot as plt
# import matplotlib.animation as anm
# import networkx as nx
import pandas as pd
import sys
import mpltern
import math
import numpy as np


### 相図のプロットを保有ポイント数で色分けする？
### Bが変化したときの均衡解の変化をベクトル場で示す．下の相図は適当なBを固定したときのものにすれば良さそう

DIR = '/home/suzuki/master/point2/' + sys.argv[1] + '/'
FONTSIZE = 15
OBJ_FNAME = 'objValue.png'
RAILCOST_FNAME = 'railCost.png'
PATHCOST_FNAME = 'pathCost.png'
POSSESS_FNAME = 'possessedPoint.png'
PATHFLOW_FNAME = 'pathFlow.png'
FLOWLINE_FNAME = 'linkFlowLine.png'
FLOWSTATIC_FNAME = 'linkFlow.png'
FLOW_FNAME = 'linkFlow.gif'
ZERO_FNAME = 'zeroFlow.png'
ONE_FNAME = 'oneFlow.png'
MONEY_FNAME = 'money.gif'
POINT_FNAME = 'point.gif'
REVENUE_FNAME = 'revenue.png'
STORE_FNAME = 'store.png'
PHASE_FNAME = 'phase.png'
MIXED_FNAME = 'priceFlow.png'
DETAIL_FNAME = 'objDetail.png'
ZEROCOST_FNAME = 'zeroCost.png'
ONECOST_FNAME = 'oneCost.png'
STEP_NUM = 2000


### 2列のデータの可視化
def make_1Dplot(fname, colname, xlabel, ylabel, savename, rev):
    df_with = pd.read_csv(DIR + fname)
    df_out = pd.read_csv(DIR + 'out' + fname)

    fig_with = plt.figure()
    fig_out = plt.figure()
    ax_with = fig_with.add_subplot(111)
    ax_out = fig_out.add_subplot(111)

    if rev:
        ax_with.plot(df_with['step'], -df_with[colname])
        ax_out.plot(df_out['step'], -df_out[colname])
    else:
        ax_with.plot(df_with['step'], df_with[colname])
        ax_out.plot(df_out['step'], df_out[colname])

    ax_with.set_xlabel(xlabel, fontsize=FONTSIZE)
    ax_with.set_ylabel(ylabel, fontsize=FONTSIZE)
    ax_out.set_xlabel(xlabel, fontsize=FONTSIZE)
    ax_with.set_ylabel(ylabel, fontsize=FONTSIZE)

    ax_with.tick_params(labelsize=FONTSIZE-1)
    ax_out.tick_params(labelsize=FONTSIZE-1)

    fig_with.subplots_adjust(left=0.16, bottom=0.14)
    fig_out.subplots_adjust(left=0.16, bottom=0.14)

    fig_with.savefig(DIR + savename)
    fig_out.savefig(DIR + 'out' + savename)

    plt.clf()
    plt.close()


### 3列以上のデータの可視化
def make_MDplot(fname, start, end, xlabel, ylabel, labels, savename):
    df_with = pd.read_csv(DIR + fname)
    df_out = pd.read_csv(DIR + 'out' + fname)

    fig_with = plt.figure()
    fig_out = plt.figure()

    ax_with = fig_with.add_subplot(111)
    ax_out = fig_out.add_subplot(111)

    if end == 0:
        end = len(df_with.columns)
    for i in range(start, end):
        ax_with.plot(df_with.iloc[:, 0], df_with.iloc[:, i])
        ax_out.plot(df_out.iloc[:, 0], df_out.iloc[:, i])
    
    if labels == []:
        ax_with.legend(fontsize=FONTSIZE-1)
        ax_out.legend(fontsize=FONTSIZE-1)
    else:
        ax_with.legend(labels=labels, fontsize=FONTSIZE-1)
        ax_out.legend(labels=labels, fontsize=FONTSIZE-1)

    ax_with.set_xlabel(xlabel, fontsize=FONTSIZE)
    ax_with.set_ylabel(ylabel, fontsize=FONTSIZE)
    ax_out.set_xlabel(xlabel, fontsize=FONTSIZE)
    ax_out.set_ylabel(ylabel, fontsize=FONTSIZE)

    ax_with.set_ylim(6, 16)
    ax_out.set_ylim(6, 16)

    ax_with.tick_params(labelsize=FONTSIZE-1)
    ax_out.tick_params(labelsize=FONTSIZE-1)

    fig_with.subplots_adjust(left=0.16, bottom=0.14)
    fig_out.subplots_adjust(left=0.16, bottom=0.14)

    fig_with.savefig(DIR + savename)
    fig_out.savefig(DIR + 'out' + savename)

    plt.clf()
    plt.close()


## 時間構造化NW上でのリンクと空間上のリンクを結びつける
def correspond(node_fname, link_fname):
    df_node = pd.read_csv(DIR + node_fname)
    df_link = pd.read_csv(DIR + link_fname)
    corres = [[df_node['i'].iloc[df_link['o'].iloc[i]], df_node['i'].iloc[df_link['d'].iloc[i]], df_node['t'].iloc[df_link['o'].iloc[i]]] for i in range(len(df_link))]

    return corres


### リンクフローの遷移の可視化
def make_linkflow_plot(fname, corres, savename):
    df_with = pd.read_csv(DIR + fname)
    df_out = pd.read_csv(DIR + 'out' + fname)

    fig_with = plt.figure()
    fig_out = plt.figure()

    ax1_with = fig_with.add_subplot(1, 3, 1)
    ax2_with = fig_with.add_subplot(1, 3, 2)
    ax3_with = fig_with.add_subplot(1, 3, 3)
    axis_with = [None, ax1_with, ax2_with, ax3_with, None]
    ax1_out = fig_out.add_subplot(1, 3, 1)
    ax2_out = fig_out.add_subplot(1, 3, 2)
    ax3_out = fig_out.add_subplot(1, 3, 3)
    axis_out = [None, ax1_out, ax2_out, ax3_out, None]

    label_with = []
    label_out = []
    for i in range(len(df_with.columns)):
        if (corres[i - 1][0] not in [0, 1, 2]) or (corres[i - 1][1] not in [0, 1, 2]):
            continue
        if corres[i - 1][2] not in [1, 2, 3]:
            continue
        axis_with[corres[i - 1][2]].plot(df_with['step'], df_with.iloc[:, i])
        label_with.append('{} to {}'.format(corres[i - 1][0], corres[i - 1][1]))
        axis_out[corres[i - 1][2]].plot(df_out['step'], df_out.iloc[:, i])
        label_out.append('{} to {}'.format(corres[i - 1][0], corres[i - 1][1]))
    
    for i in range(len(axis_with)):
        if i == 0 or 4:
            continue
        axis_with[i].legend(labels=label_with[i], fontsize=FONTSIZE-1)
        axis_with[i].set_xlabel('Days elapsed', fontsize=FONTSIZE)
        axis_with[i].set_ylabel('Link flow', fontsize=FONTSIZE)
        axis_with[i].tick_params(labelsize=FONTSIZE-1)
        axis_out[i].legend(labels=label_out[i], fontsize=FONTSIZE-1)
        axis_out[i].set_xlabel('Days elapsed', fontsize=FONTSIZE)
        axis_out[i].set_ylabel('Link flow', fontsize=FONTSIZE)
        axis_out[i].tick_params(labelsize=FONTSIZE-1)
    
    fig_with.subplots_adjust(wspace=0.16, hspace=0.14)
    fig_out.subplots_adjust(wspace=0.16, hspace=0.14)

    fig_with.savefig(DIR + savename)
    fig_out.savefig(DIR + 'out' + savename)

    plt.clf()
    plt.close()


### パスフロー（折れ線グラフ）と課金額（棒グラフ）の混合グラフの作成
def make_mixed_graph(fname_flow, fname_money, fname_point, col_line, col_bar, savename):
    df_flow_with = pd.read_csv(DIR + fname_flow)
    df_money_with = pd.read_csv(DIR + fname_money)
    df_point_with = pd.read_csv(DIR + fname_point)
    df_flow_out = pd.read_csv(DIR + 'out' + fname_flow)
    df_money_out = pd.read_csv(DIR + 'out' + fname_money)
    df_point_out = pd.read_csv(DIR + 'out' + fname_point)

    fig_with, ax_line_with = plt.subplots(1, 1)
    ax_bar_with = ax_line_with.twinx()
    fig_out, ax_line_out = plt.subplots(1, 1)
    ax_bar_out = ax_line_out.twinx()


    ax_bar_with.bar(df_money_with['step'], df_money_with.iloc[:, col_bar] - 0.7 * df_point_with.iloc[:, col_bar])
    ax_line_with.plot(df_flow_with['step'], df_flow_with.iloc[:, col_line], color='red')
    ax_bar_out.bar(df_money_out['step'], df_money_out.iloc[:, col_bar] - 0.7 * df_point_out.iloc[:, col_bar])
    ax_line_out.plot(df_flow_out['step'], df_flow_out.iloc[:, col_line], color='red')

    fig_with.savefig(DIR + savename)
    fig_out.savefig(DIR + 'out' + savename)

    plt.clf()
    plt.close()


### 積み上げグラフ作成
def make_accum_graph(fname_obj, fname_rail, fname_stay, savename):
    df_obj_with = pd.read_csv(DIR + fname_obj)
    df_rail_with = pd.read_csv(DIR + fname_rail)
    df_stay_with = pd.read_csv(DIR + fname_stay)
    df_obj_out = pd.read_csv(DIR + 'out' + fname_obj)
    df_rail_out = pd.read_csv(DIR + 'out' + fname_rail)
    df_stay_out = pd.read_csv(DIR + 'out' + fname_stay)

    df_obj_with['obj'] = -df_obj_with['sumCost']
    df_obj_with['Congestion Cost'] = -df_rail_with['railCost']
    df_obj_with['Stay Utility'] = df_stay_with['stayUtil']
    df_obj_with['Scheduling Cost'] = df_obj_with['obj'] - df_obj_with['Congestion Cost'] - df_obj_with['Stay Utility']
    df_obj_out['obj'] = -df_obj_out['sumCost']
    df_obj_out['Congestion Cost'] = -df_rail_out['railCost']
    df_obj_out['Stay Utility'] = df_stay_out['stayUtil']
    df_obj_out['Scheduling Cost'] = df_obj_out['obj'] - df_obj_out['Congestion Cost'] - df_obj_out['Stay Utility']

    df_obj_with = df_obj_with.drop(['obj', 'sumCost', 'step'], axis=1).iloc[:2001]
    df_obj_out = df_obj_out.drop(['obj', 'sumCost', 'step'], axis=1).iloc[:2001]

    fig_with = plt.figure(figsize=(16, 9))
    ax_with = fig_with.add_subplot(111)
    fig_out = plt.figure(figsize=(16, 9))
    ax_out = fig_out.add_subplot(111)

    df_obj_with.plot.bar(stacked=True, legend=False, ax=ax_with)
    df_obj_out.plot.bar(stacked=True, legend=False, ax=ax_out)

    ax_with.set_ylim(-100, 105)
    ax_out.set_ylim(-100, 105)

    ax_with.set_xlabel('Days elapsed')
    ax_with.set_ylabel('Utility')
    ax_out.set_xlabel('Days elapsed')
    ax_out.set_ylabel('Utility')

    ax_with.legend(fontsize=FONTSIZE-1)
    ax_out.legend(fontsize=FONTSIZE-1)
    ax_with.set_xticks([i for i in range(0, 2000, 100)])
    ax_out.set_xticks([i for i in range(0, 2000, 100)])
    ax_with.set_xlabel('Days elapsed', fontsize=FONTSIZE)
    ax_with.set_ylabel('Utility', fontsize=FONTSIZE)
    ax_out.set_xlabel('Days elapsed', fontsize=FONTSIZE)
    ax_out.set_ylabel('Utility', fontsize=FONTSIZE)
    ax_with.tick_params(labelsize=FONTSIZE-1)
    ax_out.tick_params(labelsize=FONTSIZE-1)

    fig_with.savefig(DIR + savename)
    fig_out.savefig(DIR + 'out' + savename)

    plt.clf()
    plt.close()


### 経路ごとに積み上げグラフ作成
def make_path_accum_graph(fname, savename, lower, upper):
    df_with = pd.read_csv(DIR + fname)
    df_out = pd.read_csv(DIR + 'out' + fname)

    df_with['obj'] = -df_with['2']
    df_with['Congestion Cost'] = -df_with['0']
    df_with['Stay Utility'] = -df_with['1']
    df_with['Fare and Points'] = -df_with['3']
    df_with['Scheduling Cost'] = df_with['obj'] - df_with['Congestion Cost'] - df_with['Stay Utility']
    df_out['obj'] = -df_out['2']
    df_out['Congestion Cost'] = -df_out['0']
    df_out['Stay Utility'] = -df_out['1']
    df_out['Fare and Points'] = -df_out['3']
    df_out['Scheduling Cost'] = df_out['obj'] - df_out['Congestion Cost'] - df_out['Stay Utility']


    df_with = df_with.drop(['0', '1', '2', '3', 'obj', 'step'], axis=1).iloc[:2001]
    df_out = df_out.drop(['0', '1', '2', '3', 'obj', 'step'], axis=1).iloc[:2001]

    fig_with = plt.figure(figsize=(16, 9))
    ax_with = fig_with.add_subplot(111)
    fig_out = plt.figure(figsize=(16, 9))
    ax_out = fig_out.add_subplot(111)

    df_with.plot.bar(stacked=True, legend=False, ax=ax_with)
    df_out.plot.bar(stacked=True, legend=False, ax=ax_out)

    ax_with.set_ylim(lower, upper)
    ax_out.set_ylim(lower, upper)

    ax_with.set_xlabel('Days elapsed')
    ax_with.set_ylabel('Utility')
    ax_out.set_xlabel('Days elapsed')
    ax_out.set_ylabel('Utility')

    ax_with.legend(fontsize=FONTSIZE-1)
    ax_out.legend(fontsize=FONTSIZE-1)
    ax_with.set_xticks([i for i in range(0, 2000, 100)])
    ax_out.set_xticks([i for i in range(0, 2000, 100)])
    ax_with.set_xlabel('Days elapsed', fontsize=FONTSIZE)
    ax_with.set_ylabel('Utility', fontsize=FONTSIZE)
    ax_out.set_xlabel('Days elapsed', fontsize=FONTSIZE)
    ax_out.set_ylabel('Utility', fontsize=FONTSIZE)
    ax_with.tick_params(labelsize=FONTSIZE-1)
    ax_out.tick_params(labelsize=FONTSIZE-1)

    fig_with.savefig(DIR + savename)
    fig_out.savefig(DIR + 'out' + savename)

    plt.clf()
    plt.close()



### 相図の作成
def make_phase_diagram(obj_fname, pathflow_fname, perturb_fname, contour_fname, contour_obj_fname, savename, poi, scale, cnum, minscale, mode, arrowpos, color='black', width=0.3, mode2=False):
    ## データの読み込み
    df_obj_with = pd.read_csv(DIR + obj_fname)
    df_obj_out = pd.read_csv(DIR + 'out' + obj_fname)
    df_obj_no = pd.read_csv(DIR + 'no' + obj_fname)
    df_flow_with = pd.read_csv(DIR + pathflow_fname)
    df_flow_out = pd.read_csv(DIR + 'out' + pathflow_fname)
    df_flow_no = pd.read_csv(DIR + 'no' + pathflow_fname)
    df_contour_with = pd.read_csv(DIR + contour_fname)
    df_contour_out = pd.read_csv(DIR + 'out' + contour_fname)
    df_contour_no = pd.read_csv(DIR + 'no' + contour_fname)
    df_contour_obj_with = pd.read_csv(DIR + contour_obj_fname)
    df_contour_obj_out = pd.read_csv(DIR + 'out' + contour_obj_fname)
    df_contour_obj_no = pd.read_csv(DIR + 'no' + contour_obj_fname)
    df_perturb_with = pd.read_csv(DIR + perturb_fname)
    df_perturb_out = pd.read_csv(DIR + 'out' + perturb_fname)
    df_perturb_no = pd.read_csv(DIR + 'no' + perturb_fname)
    if mode2:
        df_solution = pd.read_csv(DIR + 'solutionTrans.csv')

    ## 必要な部分だけ取り出す
    df_flow_with = df_flow_with[poi]
    df_flow_out = df_flow_out[poi]
    df_flow_no = df_flow_no[poi]
    df_contour_with = df_contour_with[poi]
    df_contour_out = df_contour_out[poi]
    df_contour_no = df_contour_no[poi]
    df_perturb_with = df_perturb_with[poi]
    df_perturb_out = df_perturb_out[poi]
    df_perturb_no = df_perturb_no[poi]
    if mode2:
        df_solution = df_solution[poi]

    ## 軸のスケーリング
    ternary_scale_with = df_contour_with.iloc[0].sum()
    ternary_scale_out = df_contour_out.iloc[0].sum()
    ternary_scale_no = df_contour_no.iloc[0].sum()

    ## figとaxの準備
    fig_with = plt.figure()
    fig_out = plt.figure()
    fig_no = plt.figure()
    ax_with = fig_with.add_subplot(111, projection='ternary', ternary_scale=ternary_scale_with)
    ax_out = fig_out.add_subplot(111, projection='ternary', ternary_scale=ternary_scale_out)
    ax_no = fig_no.add_subplot(111, projection='ternary', ternary_scale=ternary_scale_no)

    ## 等高線の描画
    minval = min(-df_contour_obj_with['sumCost'].max(), -df_contour_obj_out['sumCost'].max(), -df_contour_obj_no['sumCost'].max())
    maxval = max(-df_contour_obj_with['sumCost'].min(), -df_contour_obj_out['sumCost'].min(), -df_contour_obj_no['sumCost'].min())
    levels = np.linspace(minval, maxval, cnum + 1)
    cs_with = ax_with.tricontourf(df_contour_with[poi[0]], df_contour_with[poi[1]], df_contour_with[poi[2]], -df_contour_obj_with['sumCost'], levels=levels)
    cs_out = ax_out.tricontourf(df_contour_out[poi[0]], df_contour_out[poi[1]], df_contour_out[poi[2]], -df_contour_obj_out['sumCost'],levels=levels)
    cs_no = ax_no.tricontourf(df_contour_no[poi[0]], df_contour_no[poi[1]], df_contour_no[poi[2]], -df_contour_obj_no['sumCost'],levels=levels)

    if mode:
        ## mode == Trueのとき，ベクトル場の描画
        # 小さすぎるベクトルのスケーリング
        for i in range(len(df_perturb_with)):
            length_with = math.sqrt(df_perturb_with[poi[0]].iloc[i] ** 2 + df_perturb_with[poi[1]].iloc[i] ** 2 + df_perturb_with[poi[2]].iloc[i] ** 2)
            length_out = math.sqrt(df_perturb_out[poi[0]].iloc[i] ** 2 + df_perturb_out[poi[1]].iloc[i] ** 2 + df_perturb_out[poi[2]].iloc[i] ** 2)
            length_no = math.sqrt(df_perturb_no[poi[0]].iloc[i] ** 2 + df_perturb_no[poi[1]].iloc[i] ** 2 + df_perturb_no[poi[2]].iloc[i] ** 2)

            if length_with < minscale:
                df_perturb_with.iloc[i] = df_perturb_with.iloc[i] * minscale / length_with
            if length_out < minscale:
                df_perturb_out.iloc[i] = df_perturb_out.iloc[i] * minscale / length_out
            if length_no < minscale:
                df_perturb_no.iloc[i] = df_perturb_no.iloc[i] * minscale / length_no
        
        # ベクトル場の描画
        ax_with.quiver(df_contour_with[poi[0]], df_contour_with[poi[1]], df_contour_with[poi[2]], df_perturb_with[poi[0]], df_perturb_with[poi[1]], df_perturb_with[poi[2]], scale=scale, width=0.005, color='black')
        ax_out.quiver(df_contour_out[poi[0]], df_contour_out[poi[1]], df_contour_out[poi[2]], df_perturb_out[poi[0]], df_perturb_out[poi[1]], df_perturb_out[poi[2]], scale=scale, width=0.005, color='black')
        ax_no.quiver(df_contour_no[poi[0]], df_contour_no[poi[1]], df_contour_no[poi[2]], df_perturb_no[poi[0]], df_perturb_no[poi[1]], df_perturb_no[poi[2]], scale=scale, width=0.005, color='black')
    else:
        ## mode = Falseのとき，流線図の描画
        if mode2:
            ## mode2 = Trueのとき，Bがより小さいときの均衡解に点を打つ
            # ax_with.scatter(df_solution[poi[0]], df_solution[poi[1]], df_solution[poi[2]], color='black')
            ax_with.grid()
            ax_with.plot([5, 0], [0, 5], [5, 5], color='orange', lw=width+1)
            ax_out.grid()
            ax_out.plot([5, 0], [0, 5], [5, 5], color='orange', lw=width+1)
            ax_no.grid()
            ax_no.plot([5, 0], [0, 5], [5, 5], color='orange', lw=width+1)

        # 流線図の描画
        for i in range(len(df_flow_with) // (STEP_NUM + 1)):
            # プロット
            ax_with.plot(df_flow_with[poi[0]].iloc[i * STEP_NUM + i : (i + 1) * STEP_NUM + i + 1], df_flow_with[poi[1]].iloc[i * STEP_NUM + i : (i + 1) * STEP_NUM + i + 1], df_flow_with[poi[2]].iloc[i * STEP_NUM + i : (i + 1) * STEP_NUM + i + 1], color=color, lw=width)
            ax_out.plot(df_flow_out[poi[0]].iloc[i * STEP_NUM + i : (i + 1) * STEP_NUM + i + 1], df_flow_out[poi[1]].iloc[i * STEP_NUM + i : (i + 1) * STEP_NUM + i + 1], df_flow_out[poi[2]].iloc[i * STEP_NUM + i : (i + 1) * STEP_NUM + i + 1], color=color, lw=width)
            ax_no.plot(df_flow_no[poi[0]].iloc[i * STEP_NUM + i : (i + 1) * STEP_NUM + i + 1], df_flow_no[poi[1]].iloc[i * STEP_NUM + i : (i + 1) * STEP_NUM + i + 1], df_flow_no[poi[2]].iloc[i * STEP_NUM + i : (i + 1) * STEP_NUM + i + 1], color=color, lw=width)
            ax_with.scatter(df_flow_with[poi[0]].iloc[STEP_NUM - 1], df_flow_with[poi[1]].iloc[STEP_NUM - 1], df_flow_with[poi[2]].iloc[STEP_NUM - 1], color='red', s=100)
            ax_out.scatter(df_flow_out[poi[0]].iloc[STEP_NUM - 1], df_flow_out[poi[1]].iloc[STEP_NUM - 1], df_flow_out[poi[2]].iloc[STEP_NUM - 1], color='red', s=100)
            ax_no.scatter(df_flow_no[poi[0]].iloc[STEP_NUM - 1], df_flow_no[poi[1]].iloc[STEP_NUM - 1], df_flow_no[poi[2]].iloc[STEP_NUM - 1], color='red', s=100)
        
            for pos in arrowpos:
            # 矢印の追加（スケーリングしてから）
                length_with = math.sqrt(df_perturb_with[poi[0]].iloc[i * STEP_NUM + i + pos] ** 2 + df_perturb_with[poi[1]].iloc[i * STEP_NUM + i + pos] ** 2 + df_perturb_with[poi[2]].iloc[i * STEP_NUM + i + pos] ** 2)
                per0_with = df_perturb_with[poi[0]].iloc[i * STEP_NUM + i + pos] * 0.4 / length_with
                per1_with = df_perturb_with[poi[1]].iloc[i * STEP_NUM + i + pos] * 0.4 / length_with
                per2_with = df_perturb_with[poi[2]].iloc[i * STEP_NUM + i + pos] * 0.4 / length_with
                length_out = math.sqrt(df_perturb_out[poi[0]].iloc[i * STEP_NUM + i + pos] ** 2 + df_perturb_out[poi[1]].iloc[i * STEP_NUM + i + pos] ** 2 + df_perturb_out[poi[2]].iloc[i * STEP_NUM + i + pos] ** 2)
                per0_out = df_perturb_out[poi[0]].iloc[i * STEP_NUM + i + pos] * 0.4 / length_out
                per1_out = df_perturb_out[poi[1]].iloc[i * STEP_NUM + i + pos] * 0.4 / length_out
                per2_out = df_perturb_out[poi[2]].iloc[i * STEP_NUM + i + pos] * 0.4 / length_out
                length_no = math.sqrt(df_perturb_no[poi[0]].iloc[i * STEP_NUM + i + pos] ** 2 + df_perturb_no[poi[1]].iloc[i * STEP_NUM + i + pos] ** 2 + df_perturb_no[poi[2]].iloc[i * STEP_NUM + i + pos] ** 2)
                per0_no = df_perturb_no[poi[0]].iloc[i * STEP_NUM + i + pos] * 0.4 / length_no
                per1_no = df_perturb_no[poi[1]].iloc[i * STEP_NUM + i + pos] * 0.4 / length_no
                per2_no = df_perturb_no[poi[2]].iloc[i * STEP_NUM + i + pos] * 0.4 / length_no
                ax_with.quiver(df_flow_with[poi[0]].iloc[i * STEP_NUM + i + pos], df_flow_with[poi[1]].iloc[i * STEP_NUM + i + pos], df_flow_with[poi[2]].iloc[i * STEP_NUM + i + pos], per0_with, per1_with, per2_with, scale=2, width=0.005, color=color) #, headwidth=10)
                ax_out.quiver(df_flow_out[poi[0]].iloc[i * STEP_NUM + i + pos], df_flow_out[poi[1]].iloc[i * STEP_NUM + i + pos], df_flow_out[poi[2]].iloc[i * STEP_NUM + i + pos], per0_out, per1_out, per2_out, scale=2, width=0.005, color=color) #, headwidth=10)
                ax_no.quiver(df_flow_no[poi[0]].iloc[i * STEP_NUM + i + pos], df_flow_no[poi[1]].iloc[i * STEP_NUM + i + pos], df_flow_no[poi[2]].iloc[i * STEP_NUM + i + pos], per0_no, per1_no, per2_no, scale=2, width=0.005, color=color) #, headwidth=10)

    ## 等高線の凡例
    cax_with = ax_with.inset_axes([1.05, 0.1, 0.05, 0.9], transform=ax_with.transAxes)
    colorbar_with = fig_with.colorbar(cs_with, cax=cax_with)
    colorbar_with.set_label('Value of objective function', rotation=270, va='baseline')
    cax_out = ax_out.inset_axes([1.05, 0.1, 0.05, 0.9], transform=ax_out.transAxes)
    colorbar_out = fig_out.colorbar(cs_out, cax=cax_out)
    colorbar_out.set_label('Value of objective function', rotation=270, va='baseline')
    cax_no = ax_no.inset_axes([1.05, 0.1, 0.05, 0.9], transform=ax_no.transAxes)
    colorbar_no = fig_no.colorbar(cs_no, cax=cax_no)
    colorbar_no.set_label('Value of objective function', rotation=270, va='baseline')

    ## 軸の設定
    ax_with.set_tlabel('Path ψ_{}'.format(poi[0]))
    ax_with.set_llabel('Path ψ_{}'.format(poi[1]))
    ax_with.set_rlabel('Path ψ_{}'.format(poi[2]))
    ax_out.set_tlabel('Path ψ_{}'.format(poi[0]))
    ax_out.set_llabel('Path ψ_{}'.format(poi[1]))
    ax_out.set_rlabel('Path ψ_{}'.format(poi[2]))
    ax_no.set_tlabel('Path ψ_{}'.format(poi[0]))
    ax_no.set_llabel('Path ψ_{}'.format(poi[1]))
    ax_no.set_rlabel('Path ψ_{}'.format(poi[2]))
    ax_with.taxis.set_label_position('tick1')
    ax_with.laxis.set_label_position('tick1')
    ax_with.raxis.set_label_position('tick1')
    ax_out.taxis.set_label_position('tick1')
    ax_out.laxis.set_label_position('tick1')
    ax_out.raxis.set_label_position('tick1')
    ax_no.taxis.set_label_position('tick1')
    ax_no.laxis.set_label_position('tick1')
    ax_no.raxis.set_label_position('tick1')
    ax_with.tick_params(labelrotation='horizontal')
    ax_out.tick_params(labelrotation='horizontal')
    ax_no.tick_params(labelrotation='horizontal')
    fig_with.subplots_adjust(left=0.01)
    fig_out.subplots_adjust(left=0.01)
    fig_no.subplots_adjust(left=0.01)

    ## 保存
    fig_with.savefig(DIR + savename)
    fig_out.savefig(DIR + 'out' + savename)
    fig_no.savefig(DIR + 'no' + savename)


### 相図の作成
## ベクトル場の描画
make_phase_diagram('sumCostTrans.csv', 'pathFlowTrans.csv', 'contourVec.csv', 'contour.csv', 'contourObj.csv', 'vectorField.png',  ['0', '1', '2'], scale=0.4, cnum=30, minscale=0.08, mode=True, arrowpos=[1, 5, 20])
## 流線図の描画
make_phase_diagram('sumCostTrans.csv', 'stream.csv', 'streamVec.csv', 'contour.csv', 'contourObj.csv', 'flowDiagram.png', ['0', '1', '2'], scale=1, cnum=30, minscale=0.15, mode=False, arrowpos=[1, 10, 30])
## シミュレーション結果のトラジェクトリーの描画
make_phase_diagram('sumCostTrans.csv', 'pathFlowTrans.csv', 'perturbation.csv', 'contour.csv', 'contourObj.csv', 'trajectory.png', ['0', '1', '2'], scale=1, cnum=30, minscale=0.15, mode=False, arrowpos=[0, 200, 500, 700], color='black', width=3, mode2=True)
print('相図の作成完了')

### 目的関数の内訳の変化
make_accum_graph('sumCostTrans.csv', 'railCostTrans.csv', 'stayUtilTrans.csv', DETAIL_FNAME)
print('目的関数の内訳の遷移可視化完了')

### 経路ごとに総利得の内訳の変化
## lower：y軸の下限値，upper：y軸の上限値
make_path_accum_graph('zeroCostTrans.csv', ZEROCOST_FNAME, lower=-20, upper=11)
make_path_accum_graph('oneCostTrans.csv', ONECOST_FNAME, lower=-20, upper=11)
print('経路ごとの目的関数の内訳可視化完了')

# ### 課金額とパスフローの変化
# make_mixed_graph('pathFlowTrans.csv', 'moneyTrans.csv', 'pointTrans.csv', 1, 2, '0_' + MIXED_FNAME)
# make_mixed_graph('pathFlowTrans.csv', 'moneyTrans.csv', 'pointTrans.csv', 2, 5, '1_' + MIXED_FNAME)
# make_mixed_graph('pathFlowTrans.csv', 'moneyTrans.csv', 'pointTrans.csv', 3, 2, '2_' + MIXED_FNAME)
# print('課金額とフローの変化の遷移可視化完了')

# ### 目的関数の遷移
# make_1Dplot('sumCostTrans.csv', 'sumCost', 'Days elapsed', 'Value of objective function', OBJ_FNAME, True)
# print('目的関数の遷移可視化完了')


# ### 鉄道の混雑総コストの遷移
# make_1Dplot('railCostTrans.csv', 'railCost', 'Days elapsed', 'Congestion cost', RAILCOST_FNAME, False)
# print('混雑総コストの遷移可視化完了')


# ### 経路ごとのコストの遷移
# make_MDplot('pathCostTrans.csv', 1, 4, 'Days elapsed', 'Cost of each path', ['0', '1', '2'], '1_' + PATHCOST_FNAME)
# # make_MDplot('pathCostTrans.csv', 6, 0, 'Days elapsed', 'Cost of each path', [], '2_' + PATHCOST_FNAME)
# print('経路ごとのコスト可視化完了')


# ### 保有ポイント数の遷移
# # make_MDplot('possessedTrans.csv', 1, 0, 'Days elapsed', 'Possessed points', ['0 to 2', '0 to 1'], POSSESS_FNAME)
# make_1Dplot('possessedTrans.csv', '0', 'Days elapsed', 'Possessed points', POSSESS_FNAME, False)
# print('保有ポイント数の遷移可視化完了')


# ### 鉄道会社の収益の遷移
# make_1Dplot('storeRevenueTrans.csv', 'storeRevenue', 'Days elapsed', 'Revenue from stores', STORE_FNAME, False)
# print('収益の遷移可視化完了')


# ### パスフローの遷移
# make_MDplot('pathFlowTrans.csv', 1, 4, 'Days elapsed', 'Path flow', ['0', '1', '2'], '1_' + PATHFLOW_FNAME)
# # make_MDplot('pathFlowTrans.csv', 6, 0, 'Days elapsed', 'Path flow', [], '2_' + PATHFLOW_FNAME)


# ### リンクフローの遷移
# corres = correspond('node.csv', 'link.csv')
# make_linkflow_plot('linkFlowTrans.csv', corres, FLOWLINE_FNAME)


# ## 図作成（ODごとのフロー）
# def makeGraph(df, axis, corres, Graphs):
#     for i in range(0, len(df.columns) - 1):
#         if (corres[i][0] not in [0, 1, 2]) or (corres[i][1] not in [0, 1, 2]):
#             continue
#         if corres[i][0] == corres[i][1] == 0:
#             Graphs[corres[i][2]][0][3]['weight'] = Graphs[corres[i][2]][3][0]['weight'] = df.iloc[len(df) - 1, i + 1]
#         elif corres[i][0] == corres[i][1] == 1:
#             Graphs[corres[i][2]][1][4]['weight'] = Graphs[corres[i][2]][4][1]['weight'] = df.iloc[len(df) - 1, i + 1]
#         elif corres[i][0] == corres[i][1] == 2:
#             Graphs[corres[i][2]][2][5]['weight'] = Graphs[corres[i][2]][5][2]['weight'] = df.iloc[len(df) - 1, i + 1]
#         else:
#             Graphs[corres[i][2]][corres[i][0]][corres[i][1]]['weight'] = df.iloc[len(df) - 1, i + 1]

#     for i in range(len(Graphs)):
#         rail_edge_weights = [Graphs[i][u][v]['weight'] * 3 if (u, v) in [(0, 1), (1, 2)] else 0 for u, v in Graphs[i].edges]
#         stay_edge_weights = [Graphs[i][u][v]['weight'] * 3 if (u, v) in [(0, 3), (3, 0), (1, 4), (4, 1), (2, 5), (5, 2)] else 0 for u, v in Graphs[i].edges]
#         node_weights = [1000, 1000, 1000, 0, 0, 0]
#         labels = {0: 0, 1: 1, 2: 2, 3: '', 4: '', 5: ''}
#         pos = {0: [1.8, 0], 1: [2, 0], 2: [2.2, 0], 3: [1.8, 0.1], 4: [2, 0.1], 5: [2.2, 0.1]}
#         nx.draw_networkx_nodes(Graphs[i], pos, ax=axis[i], node_size=node_weights, labels=labels)
#         nx.draw_networkx_edges(Graphs[i], pos, ax=axis[i], arrows=False, width=rail_edge_weights)
#         nx.draw_networkx_edges(Graphs[i], pos, ax=axis[i], arrows=False, width=stay_edge_weights, connectionstyle='angle3')
#         axis[i].axis('off')
    
#     return axis

# ## アニメーションなしで図作る
# l = len(df_flow)
# for i in range(0, len(df_flow.columns) - 1):
#     if (corres[i][0] not in [0, 1, 2]) or (corres[i][1] not in [0, 1, 2]):
#         continue
#     if corres[i][0] == corres[i][1] == 0:
#         Graphs[corres[i][2]][0][3]['weight'] = Graphs[corres[i][2]][3][0]['weight'] = df_flow.iloc[l - 1, i + 1]
#     elif corres[i][0] == corres[i][1] == 1:
#         Graphs[corres[i][2]][1][4]['weight'] = Graphs[corres[i][2]][4][1]['weight'] = df_flow.iloc[l - 1, i + 1]
#     elif corres[i][0] == corres[i][1] == 2:
#         Graphs[corres[i][2]][2][5]['weight'] = Graphs[corres[i][2]][5][2]['weight'] = df_flow.iloc[l - 1, i + 1]
#     else:
#         Graphs[corres[i][2]][corres[i][0]][corres[i][1]]['weight'] = df_flow.iloc[l - 1, i + 1]

# for i in range(len(Graphs)):
#     rail_edge_weights = [Graphs[i][u][v]['weight'] * 3 if (u, v) in [(0, 1), (1, 2)] else 0 for u, v in Graphs[i].edges]
#     stay_edge_weights = [Graphs[i][u][v]['weight'] * 3 if (u, v) in [(0, 3), (3, 0), (1, 4), (4, 1), (2, 5), (5, 2)] else 0 for u, v in Graphs[i].edges]
#     node_weights = [1000, 1000, 1000, 0, 0, 0]
#     edge_labels = {(u, v): w['weight'] for u, v, w in Graphs[i].edges(data=True)}
#     labels = {0: 0, 1: 1, 2: 2, 3: '', 4: '', 5: ''}
#     pos = {0: [1.8, 0], 1: [2, 0], 2: [2.2, 0], 3: [1.8, 0.1], 4: [2, 0.1], 5: [2.2, 0.1]}
#     # nx.draw_networkx_labels(Graphs[i], pos, ax=axis_flow[i], edge_labels=edge_labels)
#     nx.draw_networkx_nodes(Graphs[i], pos, ax=axis_flow[i], node_size=node_weights, labels=labels)
#     nx.draw_networkx_edges(Graphs[i], pos, ax=axis_flow[i], arrows=False, width=rail_edge_weights, labels=edge_labels)
#     nx.draw_networkx_edges(Graphs[i], pos, ax=axis_flow[i], arrows=False, width=stay_edge_weights, connectionstyle='angle3')
#     axis_flow[i].axis('off')
# fig_flow.savefig(DIR + FLOWSTATIC_FNAME)

# l_out = len(df_out_flow)
# for i in range(0, len(df_out_flow.columns) - 1):
#     if (corres[i][0] not in [0, 1, 2]) or (corres[i][1] not in [0, 1, 2]):
#         continue
#     if corres[i][0] == corres[i][1] == 0:
#         Graphs[corres[i][2]][0][3]['weight'] = Graphs[corres[i][2]][3][0]['weight'] = df_out_flow.iloc[l_out - 1, i + 1]
#     elif corres[i][0] == corres[i][1] == 1:
#         Graphs[corres[i][2]][1][4]['weight'] = Graphs[corres[i][2]][4][1]['weight'] = df_out_flow.iloc[l_out - 1, i + 1]
#     elif corres[i][0] == corres[i][1] == 2:
#         Graphs[corres[i][2]][2][5]['weight'] = Graphs[corres[i][2]][5][2]['weight'] = df_out_flow.iloc[l_out - 1, i + 1]
#     else:
#         Graphs[corres[i][2]][corres[i][0]][corres[i][1]]['weight'] = df_out_flow.iloc[l_out - 1, i + 1]

# for i in range(len(Graphs)):
#     rail_edge_weights = [Graphs[i][u][v]['weight'] * 3 if (u, v) in [(0, 1), (1, 2)] else 0 for u, v in Graphs[i].edges]
#     stay_edge_weights = [Graphs[i][u][v]['weight'] * 3 if (u, v) in [(0, 3), (3, 0), (1, 4), (4, 1), (2, 5), (5, 2)] else 0 for u, v in Graphs[i].edges]
#     node_weights = [1000, 1000, 1000, 0, 0, 0]
#     edge_labels = {(u, v): w['weight'] for u, v, w in Graphs[i].edges(data=True)}
#     labels = {0: 0, 1: 1, 2: 2, 3: '', 4: '', 5: ''}
#     pos = {0: [1.8, 0], 1: [2, 0], 2: [2.2, 0], 3: [1.8, 0.1], 4: [2, 0.1], 5: [2.2, 0.1]}
#     # nx.draw_networkx_labels(Graphs[i], pos, ax=axis_out_flow[i], edge_labels=edge_labels)
#     nx.draw_networkx_nodes(Graphs[i], pos, ax=axis_out_flow[i], node_size=node_weights, labels=labels)
#     nx.draw_networkx_edges(Graphs[i], pos, ax=axis_out_flow[i], arrows=False, width=rail_edge_weights, labels=edge_labels)
#     nx.draw_networkx_edges(Graphs[i], pos, ax=axis_out_flow[i], arrows=False, width=stay_edge_weights, connectionstyle='angle3')
#     axis_out_flow[i].axis('off')
# fig_out_flow.savefig(DIR + 'out' + FLOWSTATIC_FNAME)


# def update_flow(day):
#     if day != 0:
#         for ax in axis_flow:
#             ax.cla()

#     # エッジの重み更新（リンクフロー）
#     for i in range(0, len(df_flow.columns) - 1):
#         if (corres[i][0] not in [0, 1, 2]) or (corres[i][1] not in [0, 1, 2]):
#             continue
#         if corres[i][0] == corres[i][1] == 0:
#             Graphs[corres[i][2]][0][3]['weight'] = Graphs[corres[i][2]][3][0]['weight'] = df_flow.iloc[day, i + 1]
#         elif corres[i][0] == corres[i][1] == 1:
#             Graphs[corres[i][2]][1][4]['weight'] = Graphs[corres[i][2]][4][1]['weight'] = df_flow.iloc[day, i + 1]
#         elif corres[i][0] == corres[i][1] == 2:
#             Graphs[corres[i][2]][2][5]['weight'] = Graphs[corres[i][2]][5][2]['weight'] = df_flow.iloc[day, i + 1]
#         else:
#             Graphs[corres[i][2]][corres[i][0]][corres[i][1]]['weight'] = df_flow.iloc[day, i + 1]

#     for i in range(len(Graphs)):
#         rail_edge_weights = [Graphs[i][u][v]['weight'] * 3 if (u, v) in [(0, 1), (1, 2)] else 0 for u, v in Graphs[i].edges]
#         stay_edge_weights = [Graphs[i][u][v]['weight'] * 3 if (u, v) in [(0, 3), (3, 0), (1, 4), (4, 1), (2, 5), (5, 2)] else 0 for u, v in Graphs[i].edges]
#         node_weights = [1000, 1000, 1000, 0, 0, 0]
#         labels = {0: 0, 1: 1, 2: 2, 3: '', 4: '', 5: ''}
#         pos = {0: [1.8, 0], 1: [2, 0], 2: [2.2, 0], 3: [1.8, 0.1], 4: [2, 0.1], 5: [2.2, 0.1]}
#         nx.draw_networkx_nodes(Graphs[i], pos, ax=axis_flow[i], node_size=node_weights, labels=labels)
#         nx.draw_networkx_edges(Graphs[i], pos, ax=axis_flow[i], arrows=False, width=rail_edge_weights)
#         nx.draw_networkx_edges(Graphs[i], pos, ax=axis_flow[i], arrows=False, width=stay_edge_weights, connectionstyle='angle3')
#         axis_flow[i].axis('off')


# # ani_flow = anm.FuncAnimation(fig_flow, update_flow, interval=100, frames=len(df_flow))
# # ani_flow.save(DIR + FLOW_FNAME, writer = 'imagemagick')
# print('リンクフローの遷移可視化完了')

# ## 課金額の遷移
# df_money = pd.read_csv(DIR + 'moneyTrans.csv')

# fig_money = plt.figure(figsize=(12, 4))
# ax0_money = fig_money.add_subplot(2, 3, 1)
# ax1_money = fig_money.add_subplot(2, 3, 2)
# ax2_money = fig_money.add_subplot(2, 3, 3)
# ax3_money = fig_money.add_subplot(2, 3, 4)
# ax4_money = fig_money.add_subplot(2, 3, 5)
# axis_money = [ax0_money, ax1_money, ax2_money, ax3_money, ax4_money]


# def update_money(day):
#     if day != 0:
#         for ax in axis_money:
#             ax.cla()

#     # エッジの重み更新（リンクフロー）
#     for i in range(0, len(df_money.columns) - 1):
#         if (corres[i][0] not in [0, 1, 2]) or (corres[i][1] not in [0, 1, 2]):
#             continue
#         if corres[i][0] == corres[i][1] == 0:
#             Graphs[corres[i][2]][0][3]['weight'] = Graphs[corres[i][2]][3][0]['weight'] = df_money.iloc[day, i + 1]
#         elif corres[i][0] == corres[i][1] == 1:
#             Graphs[corres[i][2]][1][4]['weight'] = Graphs[corres[i][2]][4][1]['weight'] = df_money.iloc[day, i + 1]
#         elif corres[i][0] == corres[i][1] == 2:
#             Graphs[corres[i][2]][2][5]['weight'] = Graphs[corres[i][2]][5][2]['weight'] = df_money.iloc[day, i + 1]
#         else:
#             Graphs[corres[i][2]][corres[i][0]][corres[i][1]]['weight'] = df_flow.iloc[day, i + 1]

#     for i in range(len(Graphs)):
#         edge_weights = [Graphs[i][u][v]['weight'] * 3 for u, v in Graphs[i].edges]
#         node_weights = [1000, 1000, 1000, 0, 0, 0]
#         labels = {0: 0, 1: 1, 2: 2, 3: '', 4: '', 5: ''}
#         pos = {0: [1.8, 0], 1: [2, 0], 2: [2.2, 0], 3: [1.8, 0.1], 4: [2, 0.1], 5: [2.2, 0.1]}
#         nx.draw_networkx_nodes(Graphs[i], pos, ax=axis_money[i], node_size=node_weights, labels=labels)
#         nx.draw_networkx_edges(Graphs[i], pos, ax=axis_money[i], arrows=False, width=edge_weights)
#         axis_flow[i].axis('off')


# # ani_money = anm.FuncAnimation(fig_money, update_money, interval=100, frames=len(df_money))
# # ani_money.save(DIR + MONEY_FNAME, writer = 'imagemagick')
# print('課金額の遷移可視化完了')


# ### ポイント付与額の遷移
# df_point = pd.read_csv(DIR + 'pointTrans.csv')

# fig_point = plt.figure(figsize=(12, 4))
# ax0_point = fig_point.add_subplot(2, 3, 1)
# ax1_point = fig_point.add_subplot(2, 3, 2)
# ax2_point = fig_point.add_subplot(2, 3, 3)
# ax3_point = fig_point.add_subplot(2, 3, 4)
# ax4_point = fig_point.add_subplot(2, 3, 5)
# axis_point = [ax0_point, ax1_point, ax2_point, ax3_point, ax4_point]


# def update_point(day):
#     if day != 0:
#         for ax in axis_point:
#             ax.cla()

#     # エッジの重み更新（リンクフロー）
#     for i in range(0, len(df_point.columns) - 1):
#         if (corres[i][0] not in [0, 1, 2]) or (corres[i][1] not in [0, 1, 2]):
#             continue
#         if corres[i][0] == corres[i][1] == 0:
#             Graphs[corres[i][2]][0][3]['weight'] = Graphs[corres[i][2]][3][0]['weight'] = df_point.iloc[day, i + 1]
#         elif corres[i][0] == corres[i][1] == 1:
#             Graphs[corres[i][2]][1][4]['weight'] = Graphs[corres[i][2]][4][1]['weight'] = df_point.iloc[day, i + 1]
#         elif corres[i][0] == corres[i][1] == 2:
#             Graphs[corres[i][2]][2][5]['weight'] = Graphs[corres[i][2]][5][2]['weight'] = df_point.iloc[day, i + 1]
#         else:
#             Graphs[corres[i][2]][corres[i][0]][corres[i][1]]['weight'] = df_point.iloc[day, i + 1]

#     for i in range(len(Graphs)):
#         edge_weights = [Graphs[i][u][v]['weight'] * 3 for u, v in Graphs[i].edges]
#         node_weights = [1000, 1000, 1000, 0, 0, 0]
#         labels = {0: 0, 1: 1, 2: 2, 3: '', 4: '', 5: ''}
#         pos = {0: [1.8, 0], 1: [2, 0], 2: [2.2, 0], 3: [1.8, 0.1], 4: [2, 0.1], 5: [2.2, 0.1]}
#         nx.draw_networkx_nodes(Graphs[i], pos, ax=axis_point[i], node_size=node_weights, labels=labels)
#         nx.draw_networkx_edges(Graphs[i], pos, ax=axis_point[i], arrows=False, width=edge_weights)
#         axis_flow[i].axis('off')


# # ani_point = anm.FuncAnimation(fig_point, update_point, interval=100, frames=len(df_point))
# # ani_point.save(DIR + POINT_FNAME, writer = 'imagemagick')
# print('ポイント付与額の遷移可視化完了')


# ## ネットワークデータの作成
# G0 = nx.DiGraph()
# G1 = nx.DiGraph()
# G2 = nx.DiGraph()
# G3 = nx.DiGraph()
# G4 = nx.DiGraph()
# Graphs = [G0, G1, G2, G3, G4]

# for G in Graphs:
#     G.add_nodes_from([0, 1, 2, 3, 4, 5])
#     G.add_weighted_edges_from([(0, 1, 0), (1, 2, 0), (0, 3, 0), (3, 0, 0), (1, 4, 0), (4, 1, 0), (2, 5, 0), (5, 2, 0)])

print('終了')