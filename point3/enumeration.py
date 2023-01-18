import networkx as nx
import pandas as pd

DIR = '/home/suzuki/master/point3/2/'
CORRES = {0: 19, 6: 25, 10: 26, 11: 27}

### データの読み込み
df_node = pd.read_csv(DIR + 'node.csv')
df_link = pd.read_csv(DIR + 'link.csv')
df_demand = pd.read_csv(DIR + 'demand.csv')
t_max = df_node['t'].max()
node_num = df_node['i'].max() + 1
print('データの読み込み完了')
print(f't_max = {t_max}')
print(f'node_num = {node_num}')


### グラフデータ作成
## グラフオブジェクト作成
G = nx.DiGraph()

## nodeデータの追加
nodes = list(df_node['id'])
G.add_nodes_from(nodes)

## edgeデータ作成
links = [(df_link['o'].iloc[i], df_link['d'].iloc[i]) for i in range(len(df_link))]
linkids = list(df_link['id'])
linkid_dict = dict(zip(links, linkids))
G.add_edges_from(links)
print('ネットワークデータ作成完了')
print(f'ノード数：{G.number_of_nodes()}')
print(f'エッジ数：{G.number_of_edges()}')


### 経路列挙
paths = [[] for _ in range(t_max)]
pathod = []
id = []
count = 0
for i in range(len(df_demand)):
    print(CORRES[df_demand['o'].iloc[i]], node_num*t_max+CORRES[df_demand['d'].iloc[i]])
    # print(df_demand['o'].iloc[i], node_num*t_max+df_demand['d'].iloc[i])
    for path in nx.all_simple_edge_paths(G=G, source=CORRES[df_demand['o'].iloc[i]], target=node_num*t_max+CORRES[df_demand['d'].iloc[i]], cutoff=t_max):
        # ラチ内リンクだけ通って鉄道リンクを通らないパスは除外
        # OD以外の発生・吸収ノードには行かない
        # 直前のノードに後戻りしない（滞在リンク・吸収リンク以外）
        # flag = False
        # flag_alla = True
        flag_i = False
        flag_r = False
        flag_other = False
        flag_return = False
        for j, link in enumerate(path):
            # if df_link['type'].iloc[linkid_dict[link]] == 's':
            #     flag = True
            # if df_link['type'].iloc[linkid_dict[link]] != 'a':
            #     flag_alla = False
            if df_link['type'].iloc[linkid_dict[link]] == 'i':
                flag_i = True
            if df_link['type'].iloc[linkid_dict[link]] == 'r':
                flag_r = True
            if df_link['type'].iloc[linkid_dict[path[j]]] == 'o' and j < len(path) - 1:
                if df_link['type'].iloc[linkid_dict[path[j + 1]]] != 's' and df_link['type'].iloc[linkid_dict[path[j + 1]]] != 'i' and df_link['d'].iloc[linkid_dict[path[j]]] % node_num != df_demand['d'].iloc[i]:
                    flag_other = True
                    break
            if df_link['type'].iloc[linkid_dict[path[j]]] != 's' and df_link['type'].iloc[linkid_dict[path[j]]] != 'a' and j < len(path) - 1:
                if df_link['o'].iloc[linkid_dict[path[j]]] % node_num == df_link['d'].iloc[linkid_dict[path[j + 1]]] % node_num:
                    flag_return = True
                    break
        if (flag_i and not flag_r) or flag_other or flag_return:
            continue

        for j, link in enumerate(path):
            paths[j].append(linkid_dict[link])
        pathod.append(i)
        id.append(count)
        count += 1
        if count % 100 == 0:
            print(f'{count}個目の経路列挙終了')

dict_for_df = {'id': id, 'od': pathod}
for i in range(t_max):
    dict_for_df['link{}'.format(i)] = paths[i]


### CSVで書き出し
df = pd.DataFrame(dict_for_df)
print(df.info())
print(df.head())
df.to_csv(DIR + 'path.csv', index=False)