import matplotlib.pyplot as plt
import pandas as pd
import mpltern
import numpy as np

DIR = '/home/suzuki/master/point2/'
BASE_SUBDIR = '81/'
FONTSIZE = 15


### 相図上で均衡解を比較する
def make_phase_diagram(pathflow_fname, contour_fname, contour_obj_fname, dirnums, savename, poi, cnum):
    ## データの読み込み
    # 相図のベースのためのデータを読み込む
    df_contour = pd.read_csv(DIR + BASE_SUBDIR + contour_fname)
    df_obj = pd.read_csv(DIR + BASE_SUBDIR + contour_obj_fname)

    # シナリオごとのフローの遷移を読み込む
    df_flow = pd.DataFrame([[0] * len(df_contour.columns)] * len(dirnums), columns=df_contour.columns)
    for i, dirnum in enumerate(dirnums):
        df_tmp = pd.read_csv(DIR + str(dirnum) + '/' + pathflow_fname)
        df_flow.iloc[i] = df_tmp.iloc[len(df_tmp) - 1]
    df_flow_out = pd.read_csv(DIR + BASE_SUBDIR + 'out' + pathflow_fname)
    df_flow_no = pd.read_csv(DIR + BASE_SUBDIR + 'no' + pathflow_fname)

    ## 必要な部分だけ取り出す
    df_contour = df_contour[poi]
    df_flow = df_flow[poi]
    df_flow_out = df_flow_out[poi]
    df_flow_no = df_flow_no[poi]

    ## 軸のスケーリング
    ternary_scale = df_contour.iloc[0].sum()

    ## figとaxの準備
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='ternary', ternary_scale=ternary_scale)

    ## 等高線の描画
    minval = -df_obj['sumCost'].max()
    maxval = -df_obj['sumCost'].min()
    levels = np.linspace(minval, maxval, cnum + 1)
    cs = ax.tricontourf(df_contour[poi[0]], df_contour[poi[1]], df_contour[poi[2]], -df_obj['sumCost'], levels=levels)

    ## グリッド
    ax.grid()

    ## 均衡点のプロット
    # for df_flow in df_flows:
    #     row = len(df_flow) - 1
    #     ax.scatter(df_flow[poi[0]].iloc[row], df_flow[poi[1]].iloc[row], df_flow[poi[2]].iloc[row])
    ax.plot(df_flow[poi[0]], df_flow[poi[1]], df_flow[poi[2]], marker='o', color='black')
    ax.scatter(df_flow_out[poi[0]].iloc[len(df_flow_out) - 1], df_flow_out[poi[1]].iloc[len(df_flow_out) - 1], df_flow_out[poi[2]].iloc[len(df_flow_out) - 1], s=150, color='blue')
    ax.scatter(df_flow_no[poi[0]].iloc[len(df_flow_no) - 1], df_flow_no[poi[1]].iloc[len(df_flow_no) - 1], df_flow_no[poi[2]].iloc[len(df_flow_no) - 1], s=150, color='orange')
    ax.text(df_flow[poi[0]].iloc[0] + 0.2, df_flow[poi[1]].iloc[0], df_flow[poi[2]].iloc[0], 'c=0', ha='right')
    ax.text(df_flow[poi[0]].iloc[len(df_flow) - 1] + 0.2, df_flow[poi[1]].iloc[len(df_flow) - 1], df_flow[poi[2]].iloc[len(df_flow) - 1], 'c=1', ha='right')
    ax.text(df_flow_out[poi[0]].iloc[len(df_flow_out) - 1] + 0.5, df_flow_out[poi[1]].iloc[len(df_flow_out) - 1], df_flow_out[poi[2]].iloc[len(df_flow_out) - 1], 'Without Point', ha='right')
    ax.text(df_flow_no[poi[0]].iloc[len(df_flow_no) - 1] + 0.5, df_flow_no[poi[1]].iloc[len(df_flow_no) - 1], df_flow_no[poi[2]].iloc[len(df_flow_no) - 1], 'Without Control', ha='right')

    ## 等高線の凡例
    cax = ax.inset_axes([1.05, 0.1, 0.05, 0.9], transform=ax.transAxes)
    colorbar = fig.colorbar(cs, cax=cax)
    colorbar.set_label('Value of objective function when c = 0.3', rotation=270, va='baseline')

    ## 軸の設定
    ax.set_tlabel('Path ψ_{}'.format(poi[0]))
    ax.set_llabel('Path ψ_{}'.format(poi[1]))
    ax.set_rlabel('Path ψ_{}'.format(poi[2]))
    ax.taxis.set_label_position('tick1')
    ax.laxis.set_label_position('tick1')
    ax.raxis.set_label_position('tick1')
    ax.tick_params(labelrotation='horizontal')
    fig.subplots_adjust(left=0.01)

    ## 保存
    fig.savefig(DIR + savename)



### 目的関数の内訳を比較する
def make_accum_graph(fname_obj, fname_rail, fname_stay, dirnums, savename):
    ## データ読み込み
    df_obj_tmp = pd.read_csv(DIR + BASE_SUBDIR + 'no' + fname_obj)
    df_rail_tmp = pd.read_csv(DIR + BASE_SUBDIR + 'no' + fname_rail)
    df_stay_tmp = pd.read_csv(DIR + BASE_SUBDIR + 'no' + fname_stay)
    df_obj = pd.DataFrame([[0] * len(df_obj_tmp.columns)] * (len(dirnums) + 2), columns=df_obj_tmp.columns)
    df_rail = pd.DataFrame([[0] * len(df_rail_tmp.columns)] * (len(dirnums) + 2), columns=df_rail_tmp.columns)
    df_stay = pd.DataFrame([[0] * len(df_stay_tmp.columns)] * (len(dirnums) + 2), columns=df_stay_tmp.columns)
    df_obj.iloc[0] = df_obj_tmp.iloc[len(df_obj_tmp) - 1]
    df_rail.iloc[0] = df_rail_tmp.iloc[len(df_rail_tmp) - 1]
    df_stay.iloc[0] = df_stay_tmp.iloc[len(df_stay_tmp) - 1]

    df_obj_tmp = pd.read_csv(DIR + BASE_SUBDIR + 'out' + fname_obj)
    df_rail_tmp = pd.read_csv(DIR + BASE_SUBDIR + 'out' + fname_rail)
    df_stay_tmp = pd.read_csv(DIR + BASE_SUBDIR + 'out' + fname_stay)
    df_obj.iloc[1] = df_obj_tmp.iloc[len(df_obj_tmp) - 1]
    df_rail.iloc[1] = df_rail_tmp.iloc[len(df_rail_tmp) - 1]
    df_stay.iloc[1] = df_stay_tmp.iloc[len(df_stay_tmp) - 1]

    count = 2
    for dirnum in dirnums:
        df_obj_tmp = pd.read_csv(DIR + str(dirnum) + '/' + fname_obj)
        df_rail_tmp = pd.read_csv(DIR + str(dirnum) + '/' + fname_rail)
        df_stay_tmp = pd.read_csv(DIR + str(dirnum) + '/' + fname_stay)
        df_obj.iloc[count] = df_obj_tmp.iloc[len(df_obj_tmp) - 1]
        df_rail.iloc[count] = df_rail_tmp.iloc[len(df_rail_tmp) - 1]
        df_stay.iloc[count] = df_stay_tmp.iloc[len(df_stay_tmp) - 1]
        count += 1

    ## データ整形
    df_obj['Stay Utility'] = df_stay['stayUtil']
    df_obj['Congestion Cost'] = -df_rail['railCost']
    df_obj['Delay Cost'] = -df_obj['sumCost'] - df_obj['Congestion Cost'] - df_obj['Stay Utility']
    df_obj = df_obj.drop(['sumCost', 'step'], axis=1).iloc[:2001]

    ## fig, axの準備
    fig = plt.figure(figsize=(16, 9))
    ax = fig.add_subplot(111)
    fig.subplots_adjust(bottom=0.2)

    ## 積み上げ棒グラフ作成
    df_obj.plot.bar(stacked=True, legend=False, ax=ax)

    ## 値の表示
    for i in range(len(df_obj)):
        ax.text(x=i, y=df_obj['Stay Utility'].iloc[i]/2, s='{:.2f}'.format(df_obj['Stay Utility'].iloc[i]), ha='center', fontsize=FONTSIZE-1)
        ax.text(x=i, y=df_obj['Congestion Cost'].iloc[i]/2, s='{:.2f}'.format(-df_obj['Congestion Cost'].iloc[i]), ha='center', fontsize=FONTSIZE-1)
        ax.text(x=i, y=(df_obj['Delay Cost'].iloc[i]+2*df_obj['Congestion Cost'].iloc[i])/2, s='{:.2f}'.format(-df_obj['Delay Cost'].iloc[i]), ha='center', fontsize=FONTSIZE-1)

    ## 軸などの設定
    ax.set_ylim(df_obj['Congestion Cost'].min() + df_obj['Delay Cost'].min() - 3, df_obj['Stay Utility'].max() + 3)
    ax.set_xlabel('Stay Utility Parameter c', fontsize=FONTSIZE)
    ax.set_ylabel('Utility', fontsize=FONTSIZE)
    ax.legend(fontsize=FONTSIZE-1)
    ax.set_xticks(
        [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12],
        ['Without Control', 'Without Point', '0', '0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1']
    )
    ax.set_xticklabels(['Without Control', 'Without Point', '0', '0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1'], rotation=45)
    ax.tick_params(labelsize=FONTSIZE-1)

    ## 保存 & close
    fig.savefig(DIR + savename)
    plt.clf()
    plt.close()


### 経路ごとの効用の内訳を比較する
def make_path_accum_graph(fname, dirnums, savename):
    ## データの読み込み
    df_tmp = pd.read_csv(DIR + BASE_SUBDIR + 'no' + fname)
    df = pd.DataFrame([[0] * len(df_tmp.columns)] * (len(dirnums) + 2), columns=df_tmp.columns)
    df.iloc[0] = df_tmp.iloc[len(df_tmp) - 1]
    df_tmp = pd.read_csv(DIR + BASE_SUBDIR + 'out' + fname)
    df.iloc[1] = df_tmp.iloc[len(df_tmp) - 1]
    for i, dirnum in enumerate(dirnums):
        df_tmp = pd.read_csv(DIR + str(dirnum) + '/' + fname)
        df.iloc[i + 2] = df_tmp.iloc[len(df_tmp) - 1]

    ## データ整形
    df['obj'] = -df['2']
    df['Stay Utility'] = -df['1']
    df['Congestion Cost'] = -df['0']
    df['Delay Cost'] = df['obj'] - df['Congestion Cost'] - df['Stay Utility']
    df['Fare and Points'] = -df['3']
    df = df.drop(['0', '1', '2', '3', 'obj', 'step'], axis=1).iloc[:2001]

    ## fig, ax準備
    fig = plt.figure(figsize=(16, 9))
    ax = fig.add_subplot(111)
    fig.subplots_adjust(bottom=0.2)

    ## 積み上げ棒グラフ作成
    df.plot.bar(stacked=True, legend=False, ax=ax)

    ## 値の表示
    for i in range(len(df)):
        ax.text(x=i, y=df['Stay Utility'].iloc[i]/2, s='{:.2f}'.format(df['Stay Utility'].iloc[i]), ha='center', fontsize=FONTSIZE-1)
        ax.text(x=i, y=df['Congestion Cost'].iloc[i]/2, s='{:.2f}'.format(-df['Congestion Cost'].iloc[i]), ha='center', fontsize=FONTSIZE-1)
        ax.text(x=i, y=(df['Delay Cost'].iloc[i]+2*df['Congestion Cost'].iloc[i])/2, s='{:.2f}'.format(-df['Delay Cost'].iloc[i]), ha='center', fontsize=FONTSIZE-1)
        ax.text(x=i, y=(df['Fare and Points'].iloc[i]+2*df['Delay Cost'].iloc[i]+3*df['Congestion Cost'].iloc[i])/2, s='{:.2f}'.format(-df['Fare and Points'].iloc[i]), ha='center', fontsize=FONTSIZE-1)

    ## 軸などの設定
    ax.set_ylim(df['Congestion Cost'].min() + df['Delay Cost'].min() + df['Fare and Points'].min() - 3, df['Stay Utility'].max() + 3)
    ax.set_xlabel('Stay Utility Parameter c', fontsize=FONTSIZE)
    ax.set_ylabel('Utility', fontsize=FONTSIZE)
    ax.legend(fontsize=FONTSIZE-1)
    ax.set_xticks(
        [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12],
        ['Without Control', 'Without Point', '0', '0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1']
    )
    ax.set_xticklabels(['Without Control', 'Without Point', '0', '0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1'], rotation=45)
    ax.tick_params(labelsize=FONTSIZE-1)

    ## 保存 & close
    fig.savefig(DIR + savename)
    plt.clf()
    plt.close()


### 相図の作成
make_phase_diagram(
    pathflow_fname='pathFlowTrans.csv',
    contour_fname='contour.csv',
    contour_obj_fname='contourObj.csv',
    dirnums=[78, 79, 80, 89, 81, 82, 83, 84, 85, 86, 87, 88],
    savename='1_solutionComp.png',
    poi=['0', '1', '2'],
    cnum=30
)
print('相図作成完了')


### 目的関数の内訳の表示
make_accum_graph(
    fname_obj='sumCostTrans.csv',
    fname_rail='railCostTrans.csv',
    fname_stay='stayUtilTrans.csv',
    dirnums=[78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88],
    savename='1_objDetail.png'
)
print('目的関数の内訳図示完了')


### 経路ごとの効用の内訳の表示
make_path_accum_graph(
    fname='zeroCostTrans.csv',
    dirnums=[78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88],
    savename='1_pathOneUtilDetail.png'
)
make_path_accum_graph(
    fname='oneCostTrans.csv',
    dirnums=[78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88],
    savename='1_pathTwoUtilDetail.png'
)
print('経路ごとの効用の内訳図示完了')

print('終了')