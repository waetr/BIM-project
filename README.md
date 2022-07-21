# BIM-project

实验用数据集：BlogCatalog

点数=10312 边数=667966 平均度数=64.7756 

average meeting probability = 0.0574587

## Problems and Inquiry

**问题：现在的实验方法无法在reasonable amount of time内跑出participant size > 5 且 k > 5的实验数据。**

从早上八点左右截至17：49，实验在服务器上还在size=5部分跑。剩下的size还有{10,20,50,100,200,500}。



观察实验，发现时间基本被CELF以及其advance部分占去。

对CELF的运算时间进行分析：

option 2:运行CELF A.size()次 （Initialization time为计算所有$\sigma(\{u\})$的时间）

```
	CELF done. total time = 15.234, simulation time = 15.234(Initialization time = 14.81200)

​	CELF done. total time = 16.266, simulation time = 16.266(Initialization time = 16.07900)

​	CELF done. total time = 55.531, simulation time = 55.531(Initialization time = 55.35900)

​	CELF done. total time = 49.063, simulation time = 49.063(Initialization time = 48.87500)

​	CELF done. total time = 24.359, simulation time = 24.359(Initialization time = 24.26500)
```

option 2:运行CELF advanced.

```
CELF advanced done. total time = 77.360, simulation time = 77.344(Initialization time = 67.50000)
```

**结论：CELF所有的时间基本在MC simulation上。并且在Initialization部分做的simulation最多（占总时间95%以上）。**



对MC_simulation，猜想可能是vector存取限制了性能，考虑换成数组。

实验结果：

1. -O3加速下，vector time = 4.516, array time = 4.578
2. 不加速，vector time = 30.437, array time = 22.250

**结论：没有O3的话加速明显，可惜在O3下换不换并没有什么区别**



对MC_simulation，猜想可能是meet次数太多导致性能较慢。

选定一个size=24，邻居总数=8700的集合进行simulation（仅1次）。

实验结果：在deadline=15的TC/IC模型下，平均尝试meet 547000次；在IC模型下，平均尝试activate 123000次.

分析：在IC模型，一个节点最多被其邻居节点各尝试激活1次，时间上限为$O(|V|+|E|)$; 在IC-M模型，每轮迭代一个节点都会被每个active neighbour尝试连接1次，时间上限为$O((|V|+|E|)*deadline)$。当deadline较大时，IC-M模型的simulation比IC模型慢。

残念ながら，即使现在的MC simulation已经做到了理论上的最少操作数，每次完整的simulation(r=10000)还是需要进行很多次meet。

**结论：MC_simulation没法再优化了**



观察到每进行一组数据的实验，都要运行CELF很多次，而每次CELF都要花大量时间计算$\sigma(\{u\})$。

想到可以把$\sigma(\{u\})$用数组存下来。

优化前（先做一次option 2 CELF，再做一次advanced CELF）：

```
CELF method done. total time = 161.422
CELF advanced done. total time = 75.078
```

优化后：





之前生成active participant的方式是每当size改变，就完全随机生成。这样存下的$\sigma(\{u\})$没有被充分利用。





## 运行方法

```bash
mkdir build && cd build
cmake ..
make
./exp
```



## 实验大纲

一个framework,描述option1和option2

输入为：graph, diffusion model, simulation method, set S

输出为：S的邻居节点的子集

graph: 一个图

diffusion model: IC

simulation method: MC, sketch

在option2时需要实现的IM算法：degree, pgrank, CELF

## 更新日志

2022/7/17

1. 写了estimate随机选点的邻居重合度的API。$重合程度=\frac{被两个及以上源点选中的点}{所有被选中的点}$
2. 大致比较了不同的随机数生成器的性能差异：minstd_rand（线性同余法）的速度比mt19937（马特赛特旋转）快约30%。最后选用了minstd_rand生成随机数。*原因：根据线性同余法的性质，在[0,1)范围内近似为平均分布，已经满足simulation的需要。*
3. 改善随机数并且使用-O3加速之后，MC simulation快的飞起，而且r=100和r=1000000的性能差距不大（意味着可以用r=10000正常实验了）
4. 重写了CELF。（原代码在遇到问题(*)时会出bug）

2022/7/18


写了option 2的advance版本。

算法：

1. 把active paticipate set $A$的所有邻居拿出来，记为$\N_A$。*对于每个$v\in \N_A$, $A_v$表示有邻居为$v$的active paticipate的集合。*
2. 按照某种顺序（degree, pageRank, margimal influence(CELF)）对于每个$v\in \N_A$, 尝试将$v$加入到seed里。
3. 每次尝试，任意选择一个$u\in A_v$，标记$u$多选了一个邻居，并且把$v$加入seed里。如果$u$已经选满$k$个邻居了，就把$u$从$A_v$中删除，再从$A_v-\{u\}$中任选一个作为$u$再次尝试，直到$u$还没选满或者$A_v$为空为止。如果$A_v$为空，说明$v$没有对应的active paticipate满足条件了，于是放弃$v$。
4. 把所有$v\in \N_A$依次尝试加入，算法结束。

选用了某一组$size=3$，邻居重合率$=\frac{3}{7}$的paticipate set，$k=2$，在$IC$和$TC-IC$模型上分别测试，输出见test.log。

结论：option 1结果最准确。在邻居重合率高的情况下，option 2漏掉了很多节点，而advance method选的节点数和option 1一样多，且degree,CELF选择的seed和option 1一样。

2022/7/20

修改了一些可能的bug，稍微优化了一些性能有关的细节。

写了顶层experiment方法，目前可以设置好参数直接挂到服务器上跑，输出结果重定向到output/result.out中。

result0.out为上一次本地跑出的部分实验结果。